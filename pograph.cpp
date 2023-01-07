#include "pograph.h"
#include<bitset>
#include<math.h>

pograph::Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b){}
pograph::Color& pograph::Color::operator=(const Color& other){
    this->r=other.r;
    this->g=other.g;
    this->b=other.b;
    return *this;
}

bool pograph::Color::operator==(Color other){
    return (this->r==other.r&&this->g==other.g&&this->b==other.b);
}

bool pograph::Color::operator!=(Color other){
    return (this->r!=other.r||this->g!=other.g||this->b!=other.b);
}


pograph::Point::Point(int x,int y): x(x), y(y){}
double pograph::Point::distance(Point& other){
    return sqrt(double((this->x-other.x)*(this->x-other.x)+(this->y-other.y)*(this->y-other.y)));
}

pograph::Rect::Rect(Point corner,int w,int h, Color color): corner(corner), w(w), h(h), color(color){}

pograph::Circle::Circle(Point center, int r, Color color): center(center), r(r), color(color){}

pograph::Pixel::Pixel(Color color): value(color){}

pograph::Pixel::Pixel(): value(std::monostate()){}

bool pograph::Pixel::isTransparent(){
    return std::holds_alternative<std::monostate>(value);
}
pograph::Color pograph::Pixel::color(){
    return std::get<Color>(value);
}

pograph::Sprite::Sprite(){
    std::vector<Pixel> tmp{Pixel()};
    pixels.push_back(tmp);
}

pograph::Sprite::Sprite(std::string input,pograph::Pixel transparencyColor): transparencyColor(transparencyColor){
    std::fstream f;
    std::string mnString;
    MagicNumber mn = P3;
    int width,height;
    int maxSaturation;
    f.open(input,std::ios::in);
    f>>mnString>>width>>height>>maxSaturation;
    pixels.resize(height);
    for(std::vector<Pixel>& a : pixels){
        a.resize(width);
    }
    if(mnString=="P6") mn=P6;
    switch(mn){
        case P3:
        for(int y=0;y<height;++y){
            for(int x=0;x<width;++x){
                int r,g,b;
                f>>r>>g>>b;
                Color color(r,g,b);
                if(transparencyColor.isTransparent()){
                    pixels[x][y]=Pixel(color);
                }else if(transparencyColor.color()!=color){
                    pixels[x][y]=Pixel(color);
                }else{
                    pixels[x][y]=Pixel();
                }
            }
        }
        break;
        case P6:
        char dummyByte;
        f.read(&dummyByte,1);
        for(int y=0;y<height;++y){
            for(int x=0;x<width;++x){
                uint8_t r,g,b;
                f.read(reinterpret_cast<char*>(&r),1);   
                f.read(reinterpret_cast<char*>(&g),1);   
                f.read(reinterpret_cast<char*>(&b),1);   
                Color color(r,g,b);
                if(transparencyColor.isTransparent()){
                    pixels[x][y]=Pixel(color);
                }else if(transparencyColor.color()!=color){
                    pixels[x][y]=Pixel(color);
                }else{
                    pixels[x][y]=Pixel();
                }
            }
        }
        break;

    }
    f.close();
}

pograph::pixelMatrix pograph::Sprite::getPixels(){return pixels;}

int pograph::Sprite::width(){return pixels[0].size();}
int pograph::Sprite::height(){return pixels.size();}



pograph::Canvas::Canvas(int width,int height){
    h = height;
    w = width;
    Color color(0,0,0);
    std::vector<Color> tmp(h,color);
    for(int i=0;i<w;++i){
        colors.push_back(tmp);
    }
}
pograph::Canvas::Canvas(colorMatrix colors)
    : colors(colors), w(colors.size()),h(colors[0].size()){}

pograph::colorMatrix pograph::Canvas::getColors(){return colors;}

void pograph::Canvas::setColor(int x,int y,Color color){
    colors[x][y]=color;
}

void pograph::Canvas::fillBackground(Color color){
    for(int y=0;y<h;++y){
        for(int x=0;x<w;++x){
        setColor(x,y,color);
        }
    }
}

void pograph::Canvas::drawRect(Rect& rect){
    for(int x=rect.corner.x;x<rect.corner.x+rect.w;++x){
        if(x>=0&&x<w){
            for(int y=rect.corner.y;y<rect.corner.y+rect.h;++y){
                if(y>=0&&y<h){
                    setColor(x,y,rect.color);
                }
            }
        }
    }
}
void pograph::Canvas::drawCircle(Circle& circle){
    for(int y=circle.center.y-circle.r;y<circle.center.y+circle.r;++y){
        if(y>=0&&y<h){
            for(int x=circle.center.x-circle.r;x<circle.center.x+circle.r;++x){
                if(x>=0&&x<w){
                    Point point(x,y);
                    if(circle.center.distance(point)<=circle.r){
                        setColor(x,y,circle.color);
                    }
                }
            }
        }
    }
}

void pograph::Canvas::drawSprite(Sprite& sprite, int x,int y){
    pixelMatrix pixels = sprite.getPixels();
    for(int yPos=y;yPos<y+sprite.height();++yPos){
        if(yPos>=0&&yPos<h){
            for(int xPos=x;xPos<x+sprite.width();++xPos){
                if(xPos>=0&&xPos<w){
                    if(!pixels[xPos-x][yPos-y].isTransparent()){
                        colors[xPos][yPos]=pixels[xPos-x][yPos-y].color();
                    }
                }
            }
        }
    }
}

int pograph::Canvas::width(){return w;}

int pograph::Canvas::height(){return h;}


void pograph::generatePPM(std::string output,pograph::Canvas& canvas, pograph::MagicNumber mn){
    pograph::colorMatrix colors = canvas.getColors();
    std::fstream file;
    file.open(output,std::ios::out);
    switch (mn){
        
        case pograph::P3:

        file<<"P3 "<<canvas.width()<<' '<<canvas.height()<<" 255\n";
        for(int y=0;y<canvas.height();++y){
            for(int x=0;x<canvas.width();++x){
                file<<(int)colors[x][y].r<<' '
                    <<(int)colors[x][y].g<<' '
                    <<(int)colors[x][y].b<<' ';
            }
        file<<'\n';
        }
        break;
        
        case pograph::P6:

        file<<"P6 "<<canvas.width()<<' '<<canvas.height()<<" 255\n";
        file.close();
        file.open(output, std::ios::out | std::ios::app | std::ios::binary);
        for(int y=0;y<canvas.height();++y){
            for(int x=0;x<canvas.width();++x){
                file.write(reinterpret_cast<char*>(&colors[x][y].r),sizeof(colors[x][y].r));
                file.write(reinterpret_cast<char*>(&colors[x][y].g),sizeof(colors[x][y].g));
                file.write(reinterpret_cast<char*>(&colors[x][y].b),sizeof(colors[x][y].b));
            }
        }
    }
    file.close(); 
}
