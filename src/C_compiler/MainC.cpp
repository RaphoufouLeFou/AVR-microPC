#include <inttypes.h>
#include "Header.hpp"

#include <avr/io.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>


#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define WHITE 0xFFFF

//#define FIRST_RTX
//#define LEO_RTX
#define RAPH_RENDERER
//#define RAPH_RENDERER_LINES
//#define COM_RECV 
//#define COM_SEND

#ifdef FIRST_RTX



const float ambient = 0.1;
const float thr = 0.01;

float fov = 3.141592 / 2;
float _d = atan(fov / 2) / 64;

const float rot0 = 0;
const float rot1 = -3.141592 / 4;

const int N = 16;

const float RD2 = 10000;


class Vector3 {
public:
    Vector3(float x = 0, float y = 0, float z = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3 operator-(Vector3 b) {
        return Vector3(this->x - b.x, this->y - b.y, this->z - b.z);
    }
    Vector3 operator+(Vector3 b) {
        return Vector3(this->x + b.x, this->y + b.y, this->z + b.z);
    }
    void operator+=(Vector3 b) {
        this->x += b.x;
        this->y += b.y;
        this->z += b.z;
    }

    Vector3 operator*(Vector3 b) {
        return Vector3(this->x * b.x, this->y * b.y, this->z * b.z);
    }

    Vector3 operator*(float b) {
        return Vector3(this->x * b, this->y * b, this->z * b);
    }

    Vector3 operator/(Vector3 b) {
        return Vector3(this->x / b.x, this->y / b.y, this->z / b.z);
    }


    Vector3 Vabs() {
        return Vector3(abs(this->x), abs(this->y), abs(this->z));
    }

    float length() {
        return sqrt(this->length2());
    }

    float length2() {
        return this->x * this->x + this->y * this->y + this->z * this->z;
    }

    void normalize_ip() {
        float l = this->length();
        this->x /= l;
        this->y /= l;
        this->z /= l;
    }


        
    float x;
    float y;
    float z;
};

class Obj {
public:
    Obj(Vector3 pos, Vector3 _alb, float refl) {
        this->pos = pos;
        this->_alb = _alb;
        this->refl = refl;
    }

    Vector3 alb() {
        return _alb;
    }

    Vector3 pos;
    Vector3 _alb;
    float refl;
};

class Floor {
public:
    Floor(float refl) { 
        this->refl = refl; 
    }
    Vector3 alb(Vector3 pos) {
        float c = ((int)floor(pos.x / 4) & 1) ^ ((int)floor(pos.z / 4) & 1);
        return Vector3(c, 1 - c, 0);
    }
    float dist(Vector3 pos) {
        return abs(pos.y);
    }
    
    Vector3 reflect(Vector3 pos, Vector3 mov) {
            return Vector3(mov.x, -mov.y, mov.z);
    }

    Vector3 normal(Vector3 pos) 
    {
        return Vector3(0, 1, 0);
    }
    float refl;
};

class face{
public:
    face(int n = 0, float d = 0) { this->n = n; this->d = d;}
    int n;
    float d;
};

class Cube : public Obj {
public:
    Cube(float s, Vector3 pos, Vector3 alb, float refl) : Obj(pos, alb, refl) {
        this->s = s;
    }

    face getface(Vector3 pos) {
        Vector3 sub = pos - this->pos;
        Vector3 d = sub.Vabs();
        if (d.x > d.y) {
            if (d.x > d.z) return face(0, d.x);
            return face(2, d.z);
        }
        if (d.y > d.z) return face(1, d.y);
        return face(2, d.z);
    }
    float dist(Vector3 pos) {
        float d = this->getface(pos).d;
        float all = d - this->s;
        return all;
    }
    Vector3 reflect(Vector3 pos, Vector3 mov) {
        int i = this->getface(pos).n;
        if (i == 0) return Vector3(-mov.x, mov.y, mov.z);
        if (i == 1) return Vector3(mov.x, -mov.y, mov.z);
        return Vector3(mov.x, mov.y, -mov.z);
    }
    Vector3 normal(Vector3 pos) {
        Vector3 a = pos - this->pos;
        int i = this->getface(pos).n;
        if (i == 0) return Vector3(a.x < 0 ? -1 : 1, 0, 0);
        if (i == 1) return Vector3(0, a.y < 0 ? -1 : 1, 0);
        return Vector3(0, 0, a.z < 0 ? -1 : 1);
    }
    float s;
};


Vector3 cam = Vector3(20, 5, 20);

Vector3 LIGHT = Vector3(1, 1, 1);
Vector3 light = Vector3(-2, 5, 10);
Vector3 B = Vector3(0.2, 0.2, 0.8);

Floor fl = Floor(0);

Cube cb = Cube(5, Vector3(0, 5, 0), Vector3(1, 1, 1), 1);


float det(Vector3 u, Vector3 v) { return u.x * v.x + u.y * v.y + u.z * v.z; }

Vector3 lighting(Vector3 pos, int n, Vector3 c, float d, float s) {
    //c : reflection, d : diffuse, s : specular

    Vector3 a = n == 0 ? fl.alb(pos) : cb.alb();
    float t = n == 0 ? fl.refl : cb.refl;

    if (d < ambient)  d = ambient;
    if (s < 0) s = 0;
    Vector3 res = Vector3(fmin((t * c.x) + a.x * d + s * LIGHT.x, 1), fmin((t * c.y) + a.y * d + s * LIGHT.y, 1), fmin(t * c.z + a.z * d + s * LIGHT.z, 1));
    return res;

}
   
Vector3 postpro(Vector3 pos, Vector3 col) {
    return col;
}

face get_dist(Vector3 pos) {
    float d2 = -6969;
    int type = -1;
    float d = fl.dist(pos);
    if (d <= 0) d = 0.001;
    if ((d2 == -6969) || (d < d2)) {
        d2 = d;
        type = 0;
    }
    d = cb.dist(pos);
    if (d <= 0) d = 0.001;
    if ((d2 == -6969) || (d < d2)) {
        d2 = d;
        type = 1;
    }
    if(d == 0) d = 0.001;
    return face(type, d2);
}


int count = 0;

Vector3 ray(Vector3 pos, Vector3 mov, int n) {
    if (n < 0) return B; 
    float prevd = 0;
    face ff = get_dist(pos);
    while (true) {
        ff = get_dist(pos);
        if ((ff.d < prevd) && (ff.d < thr)) break;
        //if(ff.d == 0) break;
        prevd = ff.d;
        Vector3 diff = pos - cam;
        if (diff.length2() > RD2)  break;
        pos += mov * (ff.d * 0.99);
    }
    count++;
    Vector3 col = B;
    if (ff.d < thr) {

        Vector3 newmov = ff.n == 0 ? fl.reflect(pos, mov) : cb.reflect(pos, mov);
        Vector3 reflected = ray(Vector3(pos), newmov, n - 1);
        Vector3 u = light - pos;
        Vector3 v = Vector3(mov);
        u.normalize_ip();
        v.normalize_ip();
        col = lighting(pos, ff.n, reflected, det(u, ff.n == 0 ? fl.normal(pos) : cb.normal(pos)), det(u, v));
    }
    if (n == N) return postpro(pos, col);
    return col;
}
    
int w = 128;
int h = 64;
        
int m = w * h + w ;

void pixel(uint8_t x, uint8_t y) {
    
    float a1 = rot0 + tan((y - 32) * _d);
    float a2 = rot1 + tan((x - 64) * _d);
    Vector3 pos = cam;
    Vector3 mov = Vector3(cos(a1) * sin(a2), -sin(a1), -cos(a1) * cos(a2));
    Vector3 color = ray(pos, mov, N);
    uint16_t c = (int)(color.x * 31) << 11 | (int)(color.y * 63) << 5 | (int)(color.z * 31);
    AVR_Output_Pixel(c, x, y);
    AVR_Output_Pixel(c, x, y);
    AVR_Output_Pixel(c, x, y);
}


            


int main(void){
  AVR_Init();
  AVR_Output_All(0x0000);

  //AVR_Output_Pixel(RED, 64, 32);
  int a = 0;
  for (int i = 0; i < m; i++)
  {

      a += 1;
      if (a >= m) a -= m;

      int y = a / w;
      int x = a % w;

      //AVR_Output_Pixel(GREEN, x, y);
      pixel(x, y);
      
  }
  //AVR_Output_Pixel(GREEN, 32, 32);
  
}

#endif
#ifdef LEO_RTX

#define WIDTH 128
#define HEIGHT 64

using namespace std;


class Pair {
public:
    Pair(int first = 0, float second = 0) { this->first = first; this->second = second; }
    int first;
    float second;
};

class Vec3 {
public:
    Vec3(float x = 0, float y = 0, float z = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3 operator-(Vec3 b) {
        return Vec3(this->x - b.x, this->y - b.y, this->z - b.z);
    }
    Vec3 operator+(Vec3 b) {
        return Vec3(this->x + b.x, this->y + b.y, this->z + b.z);
    }
    void operator+=(Vec3 b) {
        this->x += b.x;
        this->y += b.y;
        this->z += b.z;
    }

    Vec3 operator*(Vec3 b) {
        return Vec3(this->x * b.x, this->y * b.y, this->z * b.z);
    }

    Vec3 operator*(float b) {
        return Vec3(this->x * b, this->y * b, this->z * b);
    }

    Vec3 operator/(Vec3 b) {
        return Vec3(this->x / b.x, this->y / b.y, this->z / b.z);
    }


    Vec3 Vabs() {
        return Vec3(abs(this->x), abs(this->y), abs(this->z));
    }

    float len() {
        return sqrt(this->len2());
    }

    float len2() {
        return this->x * this->x + this->y * this->y + this->z * this->z;
    }

    void normalize() {
        float l = this->len();
        this->x /= l;
        this->y /= l;
        this->z /= l;
    }



    float x;
    float y;
    float z;
};

Vec3 BLACK = Vec3( 0, 0, 0 );
Vec3 Back = Vec3( 0.2, 0.2, 0.8 );
Vec3 LIGHT = Vec3( 1, 1, 1 );
float fov = M_PI / 2;
float m = tan(fov / 2);
float ratio = (float)WIDTH / HEIGHT;
float ambient = 0.1;
float FAR = 10000;
float thr = 0.001;

float det(Vec3 u, Vec3 v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

class Obj {
public:
    Obj(float alpha = 0, float metal = 0) {
        _metal = metal;
        _alpha = alpha;
        _type = 0;
    }

    Obj(int type, Vec3 pos, Vec3 alb, float s, float alpha, float metal) {
        _metal = metal;
        _alpha = alpha;
        _type = type;
        _s = s; // size, or radius
        _pos = pos;
        _alb = alb;
    }

    Vec3 alb(Vec3 pos) {
        if (_type == 0) {
            float c = ((int)(floor(pos.x / 4.0f)) & 1) ^ ((int)(floor(pos.z / 4.0f)) & 1);
            return { c, 1.0f - c, 0 };
        }
        return _alb;
    }

    // used for cubes
    Pair getFace(Vec3 pos) {
        Vec3 diff = pos - _pos;
        float dx = abs(diff.x), dy = abs(diff.y), dz = abs(diff.z);
        if (dx > dy) {
            if (dx > dz) {
                return { 0, dx };
            }
            return { 2, dz };
        }
        if (dy > dz) {
            return { 1, dy };
        }
        return { 2, dz };
    }

    float dist(Vec3 pos) {
        switch (_type) {
        case 0: return abs(pos.y);
        case 1: return getFace(pos).second - _s;
        case 2: return (_pos - pos).len() - _s;
        default: return 0;
        }
    }

    Vec3 reflect(Vec3 pos, Vec3 mov) {
        switch (_type) {
        case 0: return Vec3(mov.x, -mov.y, mov.z);
        case 1: {
            int i = getFace(pos).first;
            if (i == 0) return Vec3(-mov.x, mov.y, mov.z);
            if (i == 1) return Vec3(mov.x, -mov.y, mov.z);
            return Vec3(mov.x, mov.y, -mov.z);
        }
        case 2: {
            Vec3 diff = pos - _pos;
            diff.normalize();
            return diff * (det(mov, diff) * -2) + mov;
        }
        default: return BLACK;
        }
    }

    Vec3 normal(Vec3 pos) {
        switch (_type) {
        case 0: return Vec3(0, 1, 0);
        case 1: {
            Vec3 diff = pos - _pos;
            int i = getFace(pos).first;
            if (i == 0) return Vec3(diff.x < 0 ? -1 : 1, 0, 0);
            if (i == 1) return Vec3(0, diff.y < 0 ? -1 : 1, 0);
            return Vec3(0, 0, diff.z < 0 ? -1 : 1);
        }
        case 2: {
            Vec3 diff = pos - _pos;
            diff.normalize();
            return diff;
        }
        default: return BLACK;
        }
    }

    int _type; // 0: Floor, 1: Cube, 2: Ball
    Vec3 _pos;
    Vec3 _alb;
    float _s;
    float _alpha;
    float _metal;
};


class ObjPair {
public:
    ObjPair(float first = 0, Obj *second = NULL) { this->first = first; this->second = second; }
    float first;
    Obj *second;
};


// sphere
/* Vec3 light = {-2, 5, 10};
Vec3 cam = Vec3(0, 5, 20);
float rot[] = {0, 0}; */
// cube and sphere
Vec3 light = Vec3 ( 7, 5, 10 );
Vec3 cam = Vec3(20, 5, 20);
float rot[] = { 0, M_PI / 4 };
// infinite mirrors
/* Vec3 light = {0, 5, 0};
Vec3 cam = Vec3(0, 5, 0);
float rot[] = {0, 0}; */

/* Obj ball = Obj(2, Vec3(0, 5, 0), {1, 1, 1}, 5, 1, 1);
Obj flr = Obj(1, 1);
Obj* obj[] = {&ball, &flr}; */

Obj ball = Obj(2, Vec3(8, 5, -7), { 1, 0.9, 0 }, 5, 1, 1);

Obj cube = Obj(1, Vec3(0, 5, 0), Vec3( 1, 1, 1 ), 4, 1, 0);
Obj cb = Obj(1, Vec3(0, 5, 0), Vec3(1, 1, 1), 4, 1, 1);

Obj flr = Obj(1, 1);
Obj* obj[] = { &cb, &flr };
/* Obj cube1 = Obj(1, Vec3(0, 5, -20), {1, 1, 1}, 5, 0.1, 1);
Obj cube2 = Obj(1, Vec3(0, 5, 10), {1, 1, 1}, 5, 0.1, 1);
Obj cube3 = Obj(1, Vec3(-10, 5, -10), {1, 1, 1}, 5, 0.1, 1);
Obj cube4 = Obj(1, Vec3(10, 5, -10), {1, 1, 1}, 5, 0.1, 1);
Obj cube5 = Obj(1, Vec3(-10, 5, 0), {1, 1, 1}, 5, 0.1, 1);
Obj cube6 = Obj(1, Vec3(10, 5, 0), {1, 1, 1}, 5, 0.1, 1);
Obj flr = Obj(1, 0.5);
Obj* obj[] = {&cube1, &cube2, &cube3, &cube4, &cube5, &cube6, &flr}; */

int N = 2;
int s = 1;

float min2(float a, float b) {
    return a < b ? a : b;
}

Vec3 lighting(Vec3 pos, Obj* o, Vec3 col, float dif, float spec) {
    float t1 = o->_alpha;
    float t2 = o->_metal;
    Vec3 alb = o->alb(pos);
    if (dif < ambient) dif = ambient;
    if (spec < 0) spec = 0;
    return { min2(col.x * t2 + (alb.x * dif * t1 + spec * t2) * LIGHT.x, 1),
            min2(col.y * t2 + (alb.y * dif * t1 + spec * t2) * LIGHT.y, 1),
            min2(col.z * t2 + (alb.z * dif * t1 + spec * t2) * LIGHT.z, 1) };
}

Vec3 postPro(Vec3 pos, Vec3 col) {
    return col;
}

ObjPair getDist(Vec3 pos) {
    
    float d2 = -1;
    
    Obj* _o;
    for(int i = 0; i < sizeof(obj) / sizeof(obj[0]); i++) {
        Obj* o = obj[i];
        float d = o->dist(pos);
        if (d < 0) d = 0;
        if (d2 == -1 or d < d2) d2 = d, _o = o;
        
    }
    
    return ObjPair( d2, _o );
}

Vec3 ray(Vec3 pos, Vec3 mov, int n) {
    
    if (n < 0) return BLACK;
    float prevd = 0;
    float dist;
    Obj* o;
    
    while (true) {
        
        ObjPair p = getDist(pos);
        
        dist = p.first, o = p.second;
        if (dist <= prevd && dist < thr) break;
        prevd = dist;

        if ((pos - cam).len2() > FAR) break;
        pos += mov * (dist * 0.99);
    }

    Vec3 col = Back;
    if (dist < thr) {
        Vec3 newmov = o->reflect(pos, mov);
        Vec3 reflected = ray(Vec3(pos), newmov, n - 1);
        Vec3 u = light - pos;
        Vec3 v = mov;
        u.normalize();
        v.normalize();
        col = lighting(pos, o, reflected, det(u, o->normal(pos)), det(u, v));
    }

    if (n == N) return postPro(pos, col);
    return col;
}


void draw_sqr(int x, int y, Vec3 col) {
        
    int r = 31 * col.x, g = 63 * col.y, b = 31 * col.z;
    for (int j = x; j < x + s && j < WIDTH; j++){
        for (int i = y; i < y + s && i < HEIGHT; i++){
            AVR_Output_Pixel((r << 11) | (g << 5) | b, j, i);
            AVR_Output_Pixel((r << 11) | (g << 5) | b, j, i);
            AVR_Output_Pixel((r << 11) | (g << 5) | b, j, i);
        }
    }
}

void pixel(float x, float y) {
    // fisheye, supports >180° fov
    // float a1 = rot[0] + tan((y - (HEIGHT>>1))*d), a2 = rot[1] + tan((x - (WIDTH>>1))*d);
    // Vec3 mov = {cos(a1)*sin(a2), -sin(a1), -cos(a1)*cos(a2)};

    float dx = m * ratio * (x / (float)WIDTH - 0.5f), dy = -m * (y / (float)HEIGHT - 0.5f), dz = -1;
    float cos0 = cos(rot[0]), sin0 = sin(rot[0]), cos1 = cos(rot[1]), sin1 = sin(rot[1]);
    // rotate ray according to camera
    Vec3 mov = Vec3(
        dx * cos1 + dy * sin0 * sin1 + dz * cos0 * cos1,
        dy * cos0 - dz * sin0,
        -dx * sin1 + dy * sin0 * cos1 + dz * cos0 * cos1
    );
    mov.normalize();
    
    draw_sqr(x, y, ray(cam, mov, N));
    
}


int main(void){
    AVR_Init();
    AVR_Output_All(0x0000);

    AVR_Output_Pixel(RED, 64, 32);
    for (int y = 0; y < HEIGHT; y += s) {
        for (int x = 0; x < WIDTH; x += s) {
            pixel(x, y);
        }
  }

    AVR_Output_Pixel(GREEN, 32, 32);
  
}

#endif // LEO_RTX
#ifdef RAPH_RENDERER


#define WIDTH 128
#define HEIGHT 64

// You shouldn't really use this statement, but it's fine for small programs
using namespace std;

float PposX = 10;//player position X
float PposY = 0; //player position Y
float PposZ = 0; //player position Z

float ProtX = 0; //player rotation X
float ProtZ = 0; //player rotation Z

int translationSpeed = 32;
int rotationSpeed = 80;

int minAnalogInput = 0;
int maxAnalogInput = 4095;
int minMovement = -10;
int maxMovement = 10;

int renderDistance = 2000;

float fov = 80;
int D;

volatile int joyValX = 0;
volatile int joyValY = 0;

#define M_PI 3.14159265358979323846264338327950   // pi
#define DEG_TO_RAD M_PI/180.0

#define BROWN 0b1001101110101010

uint16_t texture_dirt[8][8] = {
    (0x7AA7),(0x7AA7),(0x9369),(0x59E5),(0xBC2B),(0x7AA7),(0x9369),(0x9369),
    (0x7AA7),(0x59E5),(0x59E5),(0x59E5),(0x59E5),(0x59E5),(0x59E5),(0x7AA7),
    (0x59E5),(0x9369),(0x9369),(0x59E5),(0x7AA7),(0x59E5),(0x7AA7),(0xBC2B),
    (0x7AA7),(0xBC2B),(0x9369),(0x7AA7),(0x59E5),(0x9369),(0x7AA7),(0x9369),
    (0x9369),(0x7AA7),(0x59E5),(0x7AA7),(0x7AA7),(0x7AA7),(0xBC2B),(0x9369),
    (0x7AA7),(0x9369),(0xBC2B),(0x9369),(0x9369),(0x7AA7),(0x9369),(0x59E5),
    (0x9369),(0x7AA7),(0x7AA7),(0x9369),(0x59E5),(0x59E5),(0xBC2B),(0x9369),
    (0x7AA7),(0x9369),(0x7AA7),(0x7AA7),(0x9369),(0x7AA7),(0x9369),(0x59E5),
};

bool* IsHided = NULL;

typedef struct cube {
    int centerX;
    int centerY;
    int centerZ;
    int length;
    int width;
    int height;
    float distToPlayer;
    uint16_t color;
}cube;

const int Map[55][4] = {

    // format : {x, y, z, 16bit color}
    {1, 7, 1, BROWN},
    {1, 6, 1, BROWN},
    {1, 5, 1, BROWN},
    {1, 4, 1, BROWN},
    {1, 3, 1, BROWN},
    {1, 7, -1, BROWN},
    {1, 6, -1, BROWN},
    {1, 5, -1, BROWN},
    {1, 4, -1, BROWN},
    {1, 3, -1, BROWN},
    {5, 7, 1, BROWN},
    {5, 6, 1, BROWN},
    {5, 5, 1, BROWN},
    {5, 4, 1, BROWN},
    {5, 3, 1, BROWN},
    {5, 7, -1, BROWN},
    {5, 6, -1, BROWN},
    {5, 5, -1, BROWN},
    {5, 4, -1, BROWN},
    {5, 3, -1, BROWN},
    {4, 7, -1, BROWN},
    {4, 6, -1, BROWN},
    {4, 5, -1, BROWN},
    {4, 4, -1, BROWN},
    {4, 3, -1, BROWN},
    {3, 7, -1, BROWN},
    {3, 6, -1, BROWN},
    {3, 5, -1, BROWN},
    {3, 4, -1, BROWN},
    {3, 3, -1, BROWN},
    {2, 7, -1, BROWN},
    {2, 6, -1, BROWN},
    {2, 5, -1, BROWN},
    {2, 4, -1, BROWN},
    {2, 3, -1, BROWN},
    {2, 7, 1, BROWN},
    {3, 7, 1, BROWN},
    {4, 7, 1, BROWN},
    {2, 3, 1, BROWN},
    {4, 3, 1, BROWN},
    {1, 7, 0, BROWN},
    {1, 6, 0, BROWN},
    {1, 5, 0, BROWN},
    {1, 4, 0, BROWN},
    {1, 3, 0, BROWN},
    {2, 3, 0, BROWN},
    {4, 3, 0, BROWN},
    {2, 7, 0, BROWN},
    {3, 7, 0, BROWN},
    {4, 7, 0, BROWN},
    {5, 7, 0, BROWN},
    {5, 6, 0, BROWN},
    {5, 5, 0, BROWN},
    {5, 4, 0, BROWN},
    {5, 3, 0, BROWN},

};



void AVR_C_Draw_Line(uint16_t col, int8_t AddressX1, int8_t AddressY1, int8_t AddressX2, int8_t AddressY2){
    
    uint8_t x1 = AddressX1;
    uint8_t y1 = AddressY1;
    uint8_t x2 = AddressX2;
    uint8_t y2 = AddressY2;
    uint8_t dx = x2 - x1;
    uint8_t dy = y2 - y1;
    int8_t sx = 0;
    int8_t sy = 0;
    if(dx > 0x80){
        dx = -dx;
        sx = -1;
    }else sx = 1;

    if(dy > 0x80){
        dy = -dy;
        sy = -1;
    }else sy = 1;
    int8_t err = dx - dy;

    while (x1 != x2 || y1 != y2)
    {
        if (x1 < WIDTH && x1 >= 0 && y1 < HEIGHT && y1 >= 0){
            AVR_Output_Pixel(col, x1, y1);
            AVR_Output_Pixel(col, x1, y1);
        }

        int8_t e2 = err << 1;
        if (e2 > -dy) 
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void AVR_C32_Draw_Line(uint16_t col, int AddressX1, int AddressY1, int AddressX2, int AddressY2){
    
    int x1 = AddressX1;
    int y1 = AddressY1;
    int x2 = AddressX2;
    int y2 = AddressY2;
    int dx = abs (x2 - x1);
    int dy = abs (y2 - y1);
    int sx = x2 - x1 < 0 ? -1 : 1;
    int sy = y2 - y1 < 0 ? -1 : 1;
    int err = dx - dy;

    while (x1 != x2 || y1 != y2)
    {
        if (x1 < WIDTH && x1 >= 0 && y1 < HEIGHT && y1 >= 0){
            AVR_Output_Pixel(col, x1, y1);
            AVR_Output_Pixel(col, x1, y1);
        }

        int e2 = err << 1;
        if (e2 > -dy) 
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}


int verticesConTable[12][2] = {

    {0 , 1},
    {1 , 3},
    {2 , 3},
    {2 , 0},
    {4 , 5},
    {5 , 7},
    {6 , 7},
    {6 , 4},
    {0 , 4},
    {1 , 5},
    {2 , 6},
    {3 , 7}
};

int FacesConTable[6][4] = {
    {0 , 1, 3, 2},
    {4 , 5, 7, 6},
    {2 , 3, 7, 6},
    {0 , 1, 5, 4},
    {1 , 3, 7, 5},
    {0 , 2, 6, 4}
};

/*
        2-------3 -  -  -  -  - 6-------7
        |       |               |       |
        |       | BAS           |       | HAUT
        0-------1-  -  -  -  -  4-------5
*/

int EdgesConTable[6][4] = {
    {0 , 1, 2, 3},
    {4 , 5, 6, 7},
    {0 , 9, 4, 8},
    {1 , 10, 5, 9},
    {2 , 11, 6, 10},
    {3 , 8, 7, 11}
};


// Fonction auxiliaire pour calculer l'aire d'un triangle form� par trois points
// Auxiliary function to calculate the area of a triangle formed by three points
int calculate_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}

// Fonction auxiliaire pour v�rifier si un point (x, y) est � l'int�rieur du carr�
// Auxiliary function to check if a point (x,y) is inside the square
int is_point_inside_square(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    // Utiliser l'algorithme du produit vectoriel pour v�rifier si le point est � l'int�rieur du triangle form� par les sommets du carr�
    // Use the cross product algorithm to check if the point is inside the triangle formed by the vertices of the square
    int area1 = calculate_area(x, y, x1, y1, x2, y2);
    int area2 = calculate_area(x, y, x2, y2, x3, y3);
    int area3 = calculate_area(x, y, x3, y3, x4, y4);
    int area4 = calculate_area(x, y, x4, y4, x1, y1);

    // Le point est � l'int�rieur du carr� si les aires des triangles sont toutes positives
    // The point is inside the square if the areas of the triangles are all positive
    return (area1 >= 0 && area2 >= 0 && area3 >= 0 && area4 >= 0);
}

float Mmap(float x, long in_min, long in_max, long out_min, long out_max)
{
    if (in_max == in_min) return 0;
    return ((((x - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min);
}


float calc_diff(int posX, int posY, int posZ, int playerX, int playerY, int playerZ) {
    float diffXY = (playerX - posX) * (playerX - posX) + (playerY - posY) * (playerY - posY);
    float diffTot = diffXY + (playerZ - posZ) * (playerZ - posZ);
    return diffTot;
}


void draw_square(uint16_t color, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    // Calculer les coordonn�es du rectangle englobant
    // Calculate the coordinates of the bounding rectangle


    int minX = x1;
    int maxX = x1;
    int minY = y1;
    int maxY = y1;

    if (x2 < minX) minX = x2;
    if (x2 > maxX) maxX = x2;
    if (y2 < minY) minY = y2;
    if (y2 > maxY) maxY = y2;

    if (x3 < minX) minX = x3;
    if (x3 > maxX) maxX = x3;
    if (y3 < minY) minY = y3;
    if (y3 > maxY) maxY = y3;

    if (x4 < minX) minX = x4;
    if (x4 > maxX) maxX = x4;
    if (y4 < minY) minY = y4;
    if (y4 > maxY) maxY = y4;

    int minBisX = minX;
    int minBisY = minY;
    int maxBisX = maxX;
    int maxBisY = maxY;

    if (minX >= WIDTH || minY >= HEIGHT || maxX < 0 || maxY < 0)
        return;

    if (maxX > WIDTH)
        maxX = WIDTH;

    if (maxY > HEIGHT)
        maxY = HEIGHT;

    if (minX < 0)
        minX = 0;

    if (minY < 0)
        minY = 0;

    // Draw the square


    int MaxX1X2 = x1 < x2 ? x2 : x1;
    int MinX1X2 = x1 < x2 ? x1 : x2;
    int MaxX4X3 = x4 < x3 ? x3 : x4;
    int MinX4X3 = x4 < x3 ? x4 : x3;

    for (int x = minX; x < maxX; x++) {
        for (int y = minY; y < maxY; y++) {
            if (false == false) {
                //if (is_point_inside_square(x, y, x1, y1, x2, y2, x3, y3, x4, y4) || is_point_inside_square(x, y, x2, y2, x1, y1, x4, y4, x3, y3)) {

                float mapY1 = Mmap(x, minBisX, maxBisX, MinX1X2, MaxX1X2);
                float mapY2 = Mmap(x, minBisX, maxBisX, MinX4X3, MaxX4X3);

                float u1 = (mapY1 - MinX1X2) / (MaxX1X2 - MinX1X2);
                float u2 = (mapY2 - MinX4X3) / (MaxX4X3 - MinX4X3);

                if (x1 > x2 || x4 > x3) {
                    u1 = y1 < y2 ? 1 - u1 : u1;
                    u2 = y4 < y3 ? 1 - u2 : u2;
                }
                else
                {
                    u1 = y1 < y2 ? u1 : 1 - u1;
                    u2 = y4 < y3 ? u2 : 1 - u2;
                }

                //float Ytmin = (u1 * max(y1, y2)) + (1 - u1) * min(y1, y2);
                //float Ytmax = (u2 * max(y4, y3)) + (1 - u2) * min(y4, y3);

                int MinY1Y2 = y1 < y2 ? y1 : y2;
                int MinY4Y3 = y4 < y3 ? y4 : y3;

                float Ytmin = (y1 < y2 ? y2 - y1 : y1 - y2) * u1 + MinY1Y2;
                float Ytmax = (y4 < y3 ? y3 - y4 : y4 - y3) * u2 + MinY4Y3;


                if (x1 == x4 && x3 == x2) {

                    if (y >= Ytmin && y <= Ytmax) {


                        int _x = (int)(((x - minBisX) * 7) / (maxBisX - minBisX));
                        int _y = (int)(((y - Ytmin) * 7) / (Ytmax - Ytmin));

                        AVR_Output_Pixel(texture_dirt[_x][_y], x, y);

                        /*
                        uint16_t r = 0;
                        uint16_t g = 0;
                        uint16_t b = 0;

                        float maptest = map(x, minBisX, maxBisX, Ytmin, Ytmax);
                        float maptest2 = map(maptest, Ytmin, Ytmax, 31, 0);
                        r = ((int)maptest2) << 11;

                        maptest2 = map(maptest, Ytmin, Ytmax, 0, 63);
                        g = (((int)maptest2) << 5);

                        maptest = map(y, minBisY, maxBisY, Ytmin, Ytmax);
                        maptest2 = map(maptest, Ytmin, Ytmax, 0, 31);

                        b = ((int)maptest2);

                        uint16_t colorbis = r + g + b;
                        buffer[(y * WIDTH + x)] = colorbis;
                        IsHided[y * WIDTH + x] = true;
                     */
                    }

                }

                else if (1 == 0) {
                    int MaxY1Y4 = y1 < y4 ? y4 : y1;
                    int MinY1Y4 = y1 < y4 ? y1 : y4;
                    int MaxY2Y3 = y2 < y3 ? y3 : y2;
                    int MinY2Y3 = y2 < y3 ? y2 : y3;

                    float mapX1 = Mmap(y, minBisY, maxBisY, MinY1Y4, MaxY1Y4);
                    float mapX2 = Mmap(y, minBisY, maxBisY, MinY2Y3, MaxY2Y3);

                    float t1 = (mapX1 - MinY1Y4) / (MaxY1Y4 - MinY1Y4);
                    float t2 = (mapX2 - MinY2Y3) / (MaxY2Y3 - MinY2Y3);

                    if (y1 < y4 || y2 < y3) {
                        t1 = x1 > x4 ? 1 - t1 : t1;
                        t2 = x2 > x3 ? 1 - t2 : t2;
                    }
                    else
                    {
                        t1 = x1 > x4 ? t1 : 1 - t1;
                        t2 = x2 > x3 ? t2 : 1 - t2;
                    }
                    int MinX2X3 = x2 < x3 ? x2 : x3;
                    int MinX1X4 = x1 < x4 ? x1 : x4;

                    float Xtmin = ((x1 < x4) ? x4 - x1 : x1 - x4) * t1 + MinX1X4;
                    float Xtmax = ((x2 < x3) ? x3 - x2 : x2 - x3) * t2 + MinX2X3;

                    if (x >= Xtmin && x <= Xtmax && y >= Ytmin && y <= Ytmax) {
                        /*
                        int _x = (x1 > x2 || x4 > x3) ? abs((int)map(x, Xtmin, Xtmax, 7, 0) % 8) : abs((int)map(x, Xtmin, Xtmax, 0, 7) % 8);
                        int _y = abs((int)map(y, Ytmin, Ytmax, 0, 7) % 8);
                        buffer[(y * WIDTH + x)] = texture_dirt[_x][_y];
                        IsHided[y * WIDTH + x] = true;
                        */

                        uint16_t r = 0;
                        uint16_t g = 0;
                        uint16_t b = 0;

                        float maptest = Mmap(x, minBisX, maxBisX, Ytmin, Ytmax);
                        float maptest2 = Mmap(maptest, Ytmin, Ytmax, 31, 0);
                        r = ((int)maptest2) << 11;

                        maptest2 = Mmap(maptest, Ytmin, Ytmax, 0, 63);
                        g = (((int)maptest2) << 5);

                        maptest = Mmap(y, minBisY, maxBisY, Ytmin, Ytmax);
                        maptest2 = Mmap(maptest, Ytmin, Ytmax, 0, 31);

                        b = ((int)maptest2);

                        uint16_t colorbis = r | g | b;
                        AVR_Output_Pixel(colorbis, x, y);

                    }
                }
                //}
            }
        }
    }
}

float equationDroite(float x1, float y1, float x2, float y2, float x) {
    return ((y2 - y1) / (x2 - x1)) * (x - x1) + y1;
}

void draw_face_subdivided(float Points[12][8][2], uint16_t texture[8][8], int edgesIndexs[4]) {
    for (int i = 0; i < 7; i++)
    {
        for (int ii = 0; ii < 7; ii++)
        {
            float pointX[4];
            float pointY[4];
            for (int iii = 0; iii < 2; iii++)
            {
                for (int iv = 0; iv < 2; iv++)
                {
                    float x1, y1, x2, y2, x3, y3, x4, y4;
                    if (Points[edgesIndexs[0]][0][1] > Points[edgesIndexs[0]][5][1]) {
                        x1 = Points[edgesIndexs[0]][i + iii][0];
                        y1 = Points[edgesIndexs[0]][i + iii][1];
                    }
                    else {
                        x1 = Points[edgesIndexs[0]][5 - (i + iii)][0];
                        y1 = Points[edgesIndexs[0]][5 - (i + iii)][1];
                    }
                    if (Points[edgesIndexs[1]][0][0] > Points[edgesIndexs[1]][5][0]) {
                        x2 = Points[edgesIndexs[1]][ii + iv][0];
                        y2 = Points[edgesIndexs[1]][ii + iv][1];
                    }
                    else {
                        x2 = Points[edgesIndexs[1]][5 - (ii + iv)][0];
                        y2 = Points[edgesIndexs[1]][5 - (ii + iv)][1];
                    }
                    if (Points[edgesIndexs[2]][0][1] > Points[edgesIndexs[2]][5][1]) {
                        x3 = Points[edgesIndexs[2]][i + iii][0];
                        y3 = Points[edgesIndexs[2]][i + iii][1];
                    }
                    else {
                        x3 = Points[edgesIndexs[2]][5 - (i + iii)][0];
                        y3 = Points[edgesIndexs[2]][5 - (i + iii)][1];
                    }
                    if (Points[edgesIndexs[3]][0][0] > Points[edgesIndexs[3]][5][0]) {
                        x4 = Points[edgesIndexs[3]][ii + iv][0];
                        y4 = Points[edgesIndexs[3]][ii + iv][1];
                    }
                    else {
                        x4 = Points[edgesIndexs[3]][5 - (ii + iv)][0];
                        y4 = Points[edgesIndexs[3]][5 - (ii + iv)][1];
                    }


                    // Calcul des �quations des droites
                    float m1 = (y3 - y1) / (x3 - x1);
                    float b1 = y1 - m1 * x1;

                    float m2 = (y4 - y2) / (x4 - x2);
                    float b2 = y2 - m2 * x2;

                    // Calcul du point d'intersection en r�solvant le syst�me d'�quations
                    pointX[iii + iv * 2] = (b2 - b1) / (m1 - m2);
                    pointY[iii + iv * 2] = equationDroite(x1, y1, x2, y2, pointX[iii + iv * 2]);
                }
            }
            draw_square(texture[i][ii], pointX[0], pointY[0], pointX[1], pointY[1], pointX[2], pointY[2], pointX[3], pointY[3]);
        }
    }
}


int draw_cube(cube Cube) {

    float halfLength = Cube.length / 2.0;
    float halfHeight = Cube.height / 2.0;
    float halfWidth = Cube.width / 2.0;

    // Calculate the cube's vertices
    float vertices[8][3] = {
        { Cube.centerX - halfLength, Cube.centerY - halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX + halfLength, Cube.centerY - halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX - halfLength, Cube.centerY + halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX + halfLength, Cube.centerY + halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX - halfLength, Cube.centerY - halfHeight, Cube.centerZ + halfWidth},
        { Cube.centerX + halfLength, Cube.centerY - halfHeight, Cube.centerZ + halfWidth},
        { Cube.centerX - halfLength, Cube.centerY + halfHeight, Cube.centerZ + halfWidth},
        { Cube.centerX + halfLength, Cube.centerY + halfHeight, Cube.centerZ + halfWidth}
    };

    // Calculate the cube's inner vertices
    float innerVerts[12][6][3];

    for (int i = 0; i < 12; i++) {
        float x1 = vertices[verticesConTable[i][0]][0];
        float y1 = vertices[verticesConTable[i][0]][1];
        float z1 = vertices[verticesConTable[i][0]][2];
        float x2 = vertices[verticesConTable[i][1]][0];
        float y2 = vertices[verticesConTable[i][1]][1];
        float z2 = vertices[verticesConTable[i][1]][2];

        for (int ii = 0; ii < 6; ii++) {
            float x = x1 + (x2 - x1) * ii / 6;
            float y = y1 + (y2 - y1) * ii / 6;
            float z = z1 + (z2 - z1) * ii / 6;
            innerVerts[i][ii][0] = x;
            innerVerts[i][ii][1] = y;
            innerVerts[i][ii][2] = z;
        }
    }



    float rotationXRad = ProtX * DEG_TO_RAD;
    float rotationZRad = ProtZ * DEG_TO_RAD;
    for (int i = 0; i < 8; i++) {
        // Translate vertices relative to PposX, PposY, PposZ
        float translatedX = vertices[i][0] - PposX;
        float translatedY = vertices[i][1] - PposY;
        float translatedZ = vertices[i][2] - PposZ;

        // Rotate around X axis
        float rotatedY = cos(rotationXRad) * translatedY - sin(rotationXRad) * translatedZ;
        float rotatedZ = sin(rotationXRad) * translatedY + cos(rotationXRad) * translatedZ;

        // Rotate around Z axis
        float rotatedX = cos(rotationZRad) * translatedX - sin(rotationZRad) * rotatedY;
        rotatedY = sin(rotationZRad) * translatedX + cos(rotationZRad) * rotatedY;

        // Translate vertices back to their original position
        vertices[i][0] = rotatedX + PposX;
        vertices[i][1] = rotatedY + PposY;
        vertices[i][2] = rotatedZ + PposZ;
    }

    for (int i = 0; i < 12; i++) {
        for (int ii = 0; ii < 6; ii++)
        {
            // Translate vertices relative to PposX, PposY, PposZ
            float translatedX = innerVerts[i][ii][0] - PposX;
            float translatedY = innerVerts[i][ii][1] - PposY;
            float translatedZ = innerVerts[i][ii][2] - PposZ;

            // Rotate around X axis
            float rotatedY = cos(rotationXRad) * translatedY - sin(rotationXRad) * translatedZ;
            float rotatedZ = sin(rotationXRad) * translatedY + cos(rotationXRad) * translatedZ;

            // Rotate around Z axis
            float rotatedX = cos(rotationZRad) * translatedX - sin(rotationZRad) * rotatedY;
            rotatedY = sin(rotationZRad) * translatedX + cos(rotationZRad) * rotatedY;

            // Translate vertices back to their original position
            innerVerts[i][ii][0] = rotatedX + PposX;
            innerVerts[i][ii][1] = rotatedY + PposY;
            innerVerts[i][ii][2] = rotatedZ + PposZ;
        }
    }

    int points[8][2];
    for (int i = 0; i < 8; i++) {

        // Project the vertices onto the screen

        // Calculate the relative coordinates of the vertex
        float relativeX = vertices[i][0] - PposX;
        float relativeY = vertices[i][1] - PposY;
        float relativeZ = vertices[i][2] - PposZ;

        // Check if the vertex is behind the player
        if ((relativeY) < -halfLength) return -1;
        if ((relativeY) == 0) relativeY = 0.1;
        if ((relativeY) < 0) relativeY = -relativeY;


        // Calculate the projected coordinates
        float projectedX = (D * relativeX) / relativeY;
        float projectedY = (D * relativeZ) / relativeY;

        // Convert the projected coordinates to screen space
        points[i][0] = projectedX + (WIDTH / 2);
        points[i][1] = projectedY + (HEIGHT / 2);
    }


    float InnerPoints[12][8][2];
    for (int i = 0; i < 12; i++) {
        for (int iii = 0; iii < 8; iii++)
        {
            // Project the vertices onto the screen
            // Calculate the relative coordinates of the vertex
            float relativeX = (iii == 0 || iii == 7) ? vertices[i % 8][0] - PposX : innerVerts[i][iii][0] - PposX;
            float relativeY = (iii == 0 || iii == 7) ? vertices[i % 8][1] - PposY : innerVerts[i][iii][1] - PposY;
            float relativeZ = (iii == 0 || iii == 7) ? vertices[i % 8][2] - PposZ : innerVerts[i][iii][2] - PposZ;

            // Calculate the projected coordinates
            float projectedX = (D * relativeX) / relativeY;
            float projectedY = (D * relativeZ) / relativeY;

            // Convert the projected coordinates to screen space
            InnerPoints[i][iii][0] = projectedX + (WIDTH / 2);
            InnerPoints[i][iii][1] = projectedY + (HEIGHT / 2);
        }
    }
/*
    int faceNotDoDraw[6] = { 0, 0, 0, 0, 0, 0 };

    for (int i = 0; i < 55; i++) {
        float dist = calc_diff(Cube.centerX, Cube.centerY, Cube.centerZ, Map[i][0] * 100, Map[i][1] * 100, Map[i][2] * 100);
        if (dist == 100 * 100) {

            if (Cube.centerX == Map[i][0] * 100 + 100) {
                faceNotDoDraw[5] = 1;
            }
            else if (Cube.centerX == Map[i][0] * 100 - 100) {
                faceNotDoDraw[4] = 1;
            }
            else if (Cube.centerY == Map[i][1] * 100 + 100) {
                faceNotDoDraw[3] = 1;
            }
            else if (Cube.centerY == Map[i][1] * 100 - 100) {
                faceNotDoDraw[2] = 1;
            }
            else if (Cube.centerZ == Map[i][2] * 100 + 100) {
                faceNotDoDraw[0] = 1;
            }
            else if (Cube.centerZ == Map[i][2] * 100 - 100) {
                faceNotDoDraw[1] = 1;
            }
        }
    }

    float distances[6];

    for (int i = 0; i < 6; i++) {
        distances[i] = calc_diff(
            (vertices[FacesConTable[i][0]][0] + vertices[FacesConTable[i][2]][0]) / 2,
            (vertices[FacesConTable[i][0]][1] + vertices[FacesConTable[i][2]][1]) / 2,
            (vertices[FacesConTable[i][0]][2] + vertices[FacesConTable[i][2]][2]) / 2,
            PposX, PposY, PposZ
        );
    }

    float minDist1 = distances[0];
    int maxFace1 = 0;
    float minDist2 = distances[1];
    int maxFace2 = 1;
    float minDist3 = distances[2];
    int maxFace3 = 2;

    for (int i = 1; i < 6; i++) {
        if (distances[i] < minDist1) {
            minDist1 = distances[i];
            maxFace1 = i;
        }
    }
    for (int i = 1; i < 6; i++) {
        if (i != maxFace1) {
            if (distances[i] < minDist2) {
                minDist2 = distances[i];
                maxFace2 = i;
            }
        }
    }
    for (int i = 1; i < 6; i++) {
        if (i != maxFace1 && i != maxFace2) {
            if (distances[i] < minDist3) {
                minDist3 = distances[i];
                maxFace3 = i;
            }
        }
    }

    if (faceNotDoDraw[maxFace1] == 0) {
        draw_square(Cube.color,
            points[FacesConTable[maxFace1][0]][0],
            points[FacesConTable[maxFace1][0]][1],
            points[FacesConTable[maxFace1][1]][0],
            points[FacesConTable[maxFace1][1]][1],
            points[FacesConTable[maxFace1][2]][0],
            points[FacesConTable[maxFace1][2]][1],
            points[FacesConTable[maxFace1][3]][0],
            points[FacesConTable[maxFace1][3]][1]);
        //draw_face_subdivided(InnerPoints, texture_dirt, EdgesConTable[maxFace1], buffer);
    }

    if (faceNotDoDraw[maxFace2] == 0) {
        draw_square(Cube.color,
            points[FacesConTable[maxFace2][0]][0],
            points[FacesConTable[maxFace2][0]][1],
            points[FacesConTable[maxFace2][1]][0],
            points[FacesConTable[maxFace2][1]][1],
            points[FacesConTable[maxFace2][2]][0],
            points[FacesConTable[maxFace2][2]][1],
            points[FacesConTable[maxFace2][3]][0],
            points[FacesConTable[maxFace2][3]][1]);
        //draw_face_subdivided(InnerPoints, texture_dirt, EdgesConTable[maxFace2], buffer);
    }

    if (faceNotDoDraw[maxFace3] == 0) {
        draw_square(Cube.color,
            points[FacesConTable[maxFace3][0]][0],
            points[FacesConTable[maxFace3][0]][1],
            points[FacesConTable[maxFace3][1]][0],
            points[FacesConTable[maxFace3][1]][1],
            points[FacesConTable[maxFace3][2]][0],
            points[FacesConTable[maxFace3][2]][1],
            points[FacesConTable[maxFace3][3]][0],
            points[FacesConTable[maxFace3][3]][1]);
        //draw_face_subdivided(InnerPoints, texture_dirt, EdgesConTable[maxFace3], buffer);
    }

*/

    // disabled for further testing and optimization of other functions


/*
    draw_square(Cube.color,
        points[FacesConTable[3][0]][0],
        points[FacesConTable[3][0]][1],
        points[FacesConTable[3][1]][0],
        points[FacesConTable[3][1]][1],
        points[FacesConTable[3][2]][0],
        points[FacesConTable[3][2]][1],
        points[FacesConTable[3][3]][0],
        points[FacesConTable[3][3]][1],
        buffer);



    for (int i = 0; i < 6; i++) {
          draw_square(Cube.color,
                    points[FacesConTable[i][0]][0],
                    points[FacesConTable[i][0]][1],
                    points[FacesConTable[i][1]][0],
                    points[FacesConTable[i][1]][1],
                    points[FacesConTable[i][2]][0],
                    points[FacesConTable[i][2]][1],
                    points[FacesConTable[i][3]][0],
                    points[FacesConTable[i][3]][1],
                    buffer);
    }*/

    for (int k = 0; k < 12; k++) {

        uint16_t col = WHITE;
        AVR_C32_Draw_Line(
            col,
            points[verticesConTable[k][0]][0],
            points[verticesConTable[k][0]][1],
            points[verticesConTable[k][1]][0], 
            points[verticesConTable[k][1]][1]
        );

        
    
    }
    

    

    return 0;
}



int compareCubes(const void* a, const void* b) {
    const cube* cubeA = (const cube*)a;
    const cube* cubeB = (const cube*)b;

    if (cubeA->distToPlayer < cubeB->distToPlayer) {
        return -1;
    }
    else if (cubeA->distToPlayer > cubeB->distToPlayer) {
        return 1;
    }
    else {
        return 0;
    }
}

// Function to sort and draw cubes
void sortAndDrawCubes(cube* cubes, int numCubes) {
    // Assuming the player's position is at (0, 0, 0)

    // Sort the cubes by distance
    qsort(cubes, numCubes, sizeof(cube), compareCubes);

    // Draw the sorted cubes (printing for demonstration)
    for (int i = 0; i < numCubes; i++) {
        float dist = calc_diff(cubes[i].centerX, cubes[i].centerY, cubes[i].centerZ, PposX, PposY, PposZ);
        if (dist < renderDistance * renderDistance)
            draw_cube(cubes[i]);
        //printf("Cube %d: (x=%d, y=%d, z=%d, distance=%lf)\n", i - 1, cubes[i].centerX, cubes[i].centerY, cubes[i].centerZ, cubes[i].distToPlayer);
    }
}


int Build_terrain() {

    // color code : G2, G1, G0, B4, B3, B2, B1, B0, R4, R3, R2, R1, R0, G5, G4, G3
    
    AVR_Draw_Fill_Rect(0b1000011000000010, 0, 0, WIDTH, HEIGHT / 2);
    AVR_Draw_Fill_Rect(0b0111011010111101, 0, HEIGHT / 2, WIDTH, HEIGHT / 2);
    
    /*

    
    uint16_t bite = 0b1000011000000010; //(sry)
    for (int x = 0; x < WIDTH; x++) {
        for (int y = HEIGHT / 2; y < HEIGHT; y++) {
            
            AVR_Output_Pixel(bite, x, y);
            AVR_Output_Pixel(bite, x, y);
        }
    }
    
    bite = 0b0111011010111101;
    uint16_t bite2 = 0b1001011100011101;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT / 2; y++) {
            //memcpy(buffer + y * WIDTH + x, &bite, sizeof(uint16_t));
            AVR_Output_Pixel(bite, x, y);
            AVR_Output_Pixel(bite, x, y);
        }
        //memcpy(buffer + i, &bite, sizeof(uint16_t));

    }
    */
    //memset(buffer, bite, HEIGHT * WIDTH);
    return 0;
}

int DrawHouse() {

    cube cubes[55];


    for (int i = 0; i < 55; i++) {

        cubes[i].centerX = Map[i][0] * 100;
        cubes[i].centerY = Map[i][1] * 100;
        cubes[i].centerZ = Map[i][2] * 100;
        cubes[i].height = cubes[i].length = cubes[i].width = 100;
        cubes[i].color = Map[i][3];
        cubes[i].distToPlayer = calc_diff(cubes[i].centerX, cubes[i].centerY, cubes[i].centerZ, PposX, PposY, PposZ);

    }

    sortAndDrawCubes(cubes, 55);
    return 0;
}


void displaytasks() {

    //memset(IsHided, 0, WIDTH * HEIGHT * sizeof(uint8_t));

    int currentMillis = 1;
    int currentMillisGlobal = 1;

    int lastMillis = 1;
    int lastMillisGlobal = 1;

    int elapsedMillisGlobal = 1;
    int elapsedMillisInput = 1;
    int elapsedMillisMemset = 1;
    int elapsedMillisCalculs1 = 1;
    int elapsedMillisCalculs2 = 1;
    int elapsedMillisCalculs3 = 1;
    int elapsedMillisDraw = 1;
    float fps = 16;
    bool IsEscaped = false;
    bool lastIsEscaped = false;
    int LastMouseX = 0;
    //int nbFrame = 0;


    D = WIDTH / 2 / tan(fov / 2 * DEG_TO_RAD);

    while (1) {

        Build_terrain();
        //SDL_SetRenderDrawColor(renderer, 115, 118, 83, 255);
        //DrawHouse();
        cube Cube;
        Cube.centerX = 100;
        Cube.centerY = 100;
        Cube.centerZ = 000;
        Cube.height = Cube.length = Cube.width = 100;
        Cube.color = BROWN;
        Cube.distToPlayer = calc_diff(Cube.centerX, Cube.centerY, Cube.centerZ, PposX, PposY, PposZ);

        
        

        uint16_t color = 0;


        int KeyState = AVR_Read_Inputs();

        if (KeyState == 0x20) {
            PposX += translationSpeed * sin(ProtZ * DEG_TO_RAD);
            PposY += translationSpeed * cos(ProtZ * DEG_TO_RAD);
        }
        if (KeyState == 0x80) {
            PposX -= translationSpeed * sin(ProtZ * DEG_TO_RAD);
            PposY -= translationSpeed * cos(ProtZ * DEG_TO_RAD);
        }
        if (KeyState == 0x40) {
            PposX -= translationSpeed * cos(ProtZ * DEG_TO_RAD);
            PposY += translationSpeed * sin(ProtZ * DEG_TO_RAD);
        }
        if (KeyState == 0x10) {
            PposX += translationSpeed * cos(ProtZ * DEG_TO_RAD);
            PposY -= translationSpeed * sin(ProtZ * DEG_TO_RAD);
        }
        if (KeyState == 0x04) {
            PposZ += translationSpeed;
        }
        if (KeyState == 0x08) {
            PposZ -= translationSpeed;
        }
        _delay_ms(20.0);
        draw_cube(Cube);
    }
}

void print(char * str){
    return;
    for (int i = 0; i < strlen(str); i++){
        break;
        sendUART(str[i]);
    }
        
}

int main(void) {

    AVR_Init();
    AVR_Output_All(0);
    displaytasks();

    //_delay_ms(2000.0);

    //AVR_StartSerial(9600);
    
    //sendUART('a');

    //Build_terrain();

    //AVR_C_Draw_Line(RED, 16, 0, 32, 32);
    //AVR_Draw_Line(BLUE, 16, 16, 32, 32);
    //print("test");

    //AVR_C_Draw_Line(RED, 16, 0, 32, 32);

    //return 0;
}


#endif // RAPH_RENDERER

#ifdef RAPH_RENDERER_LINES




#define SCALE 10

#define HEIGHT_NOT_SCALED 64
#define WIDTH_NOT_SCALED 128

#define HEIGHT 64
#define WIDTH 128


#define FOV 40



int16_t PlayerX = 0;
int16_t PlayerY = 0;
int16_t PlayerZ = 0;


void AVR_C_Draw_Line(uint16_t col, int AddressX1, int AddressY1, int AddressX2, int AddressY2){
    
    int x1 = AddressX1;
    int y1 = AddressY1;
    int x2 = AddressX2;
    int y2 = AddressY2;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int sx = 0;
    int sy = 0;
    if(dx < 0){
        dx = -dx;
        sx = -1;
    }else sx = 1;

    if(dy < 0){
        dy = -dy;
        sy = -1;
    }else sy = 1;
    int err = dx - dy;

    while (x1 != x2 || y1 != y2)
    {
        if (x1 < WIDTH && x1 >= 0 && y1 < HEIGHT && y1 >= 0){
            AVR_Output_Pixel(col, x1, y1);
            AVR_Output_Pixel(col, x1, y1);
        }


        int e2 = err << 1;
        if (e2 > -dy) 
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

int area(int x1, int x2, int x3, int y1, int y2, int y3) {
    return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2);
}


void Fill_face(int16_t PointXs[4], int16_t PointYs[4]) {

    for (int i = PointXs[0]; i <= PointXs[1]; i++)
    {
        for (int j = PointYs[0]; j <= PointYs[4]; j++)
        {
            if (i >= 0 && i < WIDTH && j >= 0 && j < HEIGHT)
                AVR_Output_Pixel(GREEN, i, j);
        }
    }

    /*
    uint8_t Case = (PointXs[1] == PointXs[4]) ? ((PointYs[0] == PointYs[1]) ? 0 : 2) : 1;

    uint8_t MaxIndeX = (PointXs[1] == PointXs[4]) ? 1 : (PointXs[1] > PointXs[4] ? 1 : 4);

    int MaxX = (PointXs[1] == PointXs[4]) ? PointXs[1] : (PointXs[1] > PointXs[6] ? PointXs[1] : PointXs[4]);
    int MinX = (PointXs[0] == PointXs[5]) ? PointXs[0] : (PointXs[0] > PointXs[5] ? PointXs[5] : PointXs[0]);

    int MaxY = (PointYs[0] == PointYs[1]) ? PointYs[0] : (PointYs[0] > PointYs[1] ? PointYs[0] : PointYs[1]);
    int MinY = (PointYs[4] == PointYs[5]) ? PointYs[4] : (PointYs[4] > PointYs[5] ? PointYs[5] : PointYs[4]);

    int FixtY = PointYs[MaxIndeX == 1 ? 2 : 0];
    int Fixt2Y = PointYs[MaxIndeX == 1 ? 4 : 1];

    switch (Case) {
    case 0:
        for (int i = MinX; i < MaxX; i++) {
            for (int j = MinY; j < MaxY; j++) {
                if (i > 0 && i < WIDTH && j > 0 && j < HEIGHT)
                    Draw_Point(i, j, 0, 255, 0);
            }
        }
        break;
    case 1:
        
        for (int i = PointXs[MaxIndeX == 1 ? 2 : 0]; i < PointXs[MaxIndeX == 1 ? 3 : 1]; i++) {
            for (int j = MaxY; j < MinY; j++) {
				if (i >= 0 && i < WIDTH && j >= 0 && j < HEIGHT)
					Draw_Point(i, j, 0, 255, 0);
			}
		}

        break;
    case 2:

        break;

    default:
        break;
    }

    //triangle 1
    int area1 = area(PointXs[0], PointXs[1], PointXs[2], PointYs[0], PointYs[1], PointYs[2]);

    

    int areaTrig1 = area(PointXs[0], PointXs[1], PointXs[2], PointYs[0], PointYs[1], PointYs[2]);

    int area2 = area(PointXs[0], PointXs[3], PointXs[2], PointYs[0], PointYs[3], PointYs[2]);
    //printf("Main rect area = %d\n", area1 + area2);
    */
}


void drawCube(int8_t PosX, int8_t PosY, int8_t PosZ, int8_t SizeX, int8_t SizeY, int8_t SizeZ) {

    uint16_t diff = PosY - PlayerY;
    if (diff < 0 || diff > 100) {
        return;
    }
    int8_t halfSizeX = SizeX >> 1;
    int8_t halfSizeY = SizeY >> 1;
    int8_t halfSizeZ = SizeZ >> 1;

    int16_t VertexX[8] = {
        PosX - halfSizeX,
        PosX + halfSizeX,
        PosX - halfSizeX,
        PosX + halfSizeX,
        PosX - halfSizeX,
        PosX + halfSizeX,
        PosX - halfSizeX,
        PosX + halfSizeX,
    };
    int16_t VertexY[8] = {
        PosY - halfSizeY,
        PosY - halfSizeY,
        PosY + halfSizeY,
        PosY + halfSizeY,
        PosY - halfSizeY,
        PosY - halfSizeY,
        PosY + halfSizeY,
        PosY + halfSizeY,
    };
    int16_t VertexZ[8] = {
        PosZ - halfSizeZ,
        PosZ - halfSizeZ,
        PosZ - halfSizeZ,
        PosZ - halfSizeZ,
        PosZ + halfSizeZ,
        PosZ + halfSizeZ,
        PosZ + halfSizeZ,
        PosZ + halfSizeZ,
    };

    int16_t PointsX[8];
    int16_t PointsY[8];

    int8_t itt = 0;

    while (itt < 8) {
        int16_t VertexXtmp = VertexX[itt];
        int16_t VertexYtmp = VertexY[itt];
        int16_t VertexZtmp = VertexZ[itt];

        VertexXtmp -= PlayerX;
        VertexYtmp -= PlayerY;
        VertexZtmp -= PlayerZ;
        
        if (VertexYtmp == 0) VertexYtmp = 1;
        if (VertexYtmp < 0) VertexYtmp = -VertexYtmp;


        int8_t temp4 = 0;
        if (VertexXtmp < 0x80) {
            VertexXtmp = -VertexXtmp;
            temp4++;
        }

        int16_t res = FOV * VertexXtmp;

        int16_t DivRes = res / VertexYtmp;

        if (temp4 == 1) DivRes = -DivRes;

        DivRes += WIDTH_NOT_SCALED /2;
        
        PointsX[itt] = DivRes;

        temp4 = 0;
        if (VertexZtmp < 0x80) {
            VertexZtmp = -VertexZtmp;
            temp4++;
        }

        res = FOV * VertexZtmp;

        DivRes = res / VertexYtmp;

        if (temp4 == 1) DivRes = -DivRes;

        DivRes += HEIGHT_NOT_SCALED/2;

        PointsY[itt] = DivRes;

        itt++;
    }

 
    int verticesConTable[12][2] = {

    {0 , 1},
    {1 , 3},
    {2 , 3},
    {2 , 0},
    {4 , 5},
    {5 , 7},
    {6 , 7},
    {6 , 4},
    {0 , 4},
    {1 , 5},
    {2 , 6},
    {3 , 7}
    };
    /*
    Fill_face(PointsX, PointsY);
*/
    for (int i = 0; i < 12; i++) {
        uint16_t col = WHITE;
        AVR_C_Draw_Line(
            col,
            PointsX[verticesConTable[i][0]],
            PointsY[verticesConTable[i][0]],
            PointsX[verticesConTable[i][1]], 
            PointsY[verticesConTable[i][1]]
        );
    }
    
    

    for (int8_t i = 0; i < 8; i++)
    {
        if ((PointsX[i] & 0xFF00) == 0 && (PointsY[i] & 0xFF00) == 0)
            AVR_Output_Pixel(RED, PointsX[i], PointsY[i]);
    }

}


int main(int argc, char** args)
{
    AVR_Init();
    AVR_Output_All(0);


    while (1) {
   
        AVR_Output_All(0);
        
        drawCube(0, 20, 0, 10, 10, 10);
        AVR_Delay(10000);
        
        

        int KeyState = AVR_Read_Inputs();

        if (KeyState == 0x10) {
            PlayerX ++;
        }
        if (KeyState == 0x40) {
            PlayerX --;
        }
        if (KeyState == 0x20) {
            PlayerY ++;
        }
        if (KeyState == 0x80) {
            PlayerY --;
        }
        if (KeyState == 0x04) {
            PlayerZ ++;
        }
        if (KeyState == 0x08) {
            PlayerZ --;
        }
    }
}



#endif // RAPH_RENDERER_LINES