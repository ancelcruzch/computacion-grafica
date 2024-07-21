#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GLUT/glut.h>
#include <iostream>
#include <cmath>
#include <functional>

using namespace std;

void setPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glFlush();
}

enum LineStyle { Solid = 1, Dotted = 2, Dashed = 3 };

void drawDDALine(int x1, int y1, int x2, int y2, LineStyle style) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));

    float xIncrement = dx / static_cast<float>(steps);
    float yIncrement = dy / static_cast<float>(steps);

    float x = x1;
    float y = y1;

    auto drawPixel = [&](int step) {
        if (style == Solid) {
            setPixel(round(x), round(y));
        } else if (style == Dotted && step % 2 == 0) {
            setPixel(round(x), round(y));
        } else if (style == Dashed && (step / 10) % 2 == 0) {
            setPixel(round(x), round(y));
        }
    };

    for (int i = 0; i <= steps; i++) {
        drawPixel(i);
        x += xIncrement;
        y += yIncrement;
    }
}

void drawBresenhamLine(int x1, int y1, int x2, int y2, LineStyle style) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    int step = 0; // Se utiliza para controlar el estilo
    while (true) {
        if (style == Solid) {
            setPixel(x1, y1);
        } else if (style == Dotted && step % 2 == 0) {
            setPixel(x1, y1);
        } else if (style == Dashed && (step / 10) % 2 == 0) {
            setPixel(x1, y1);
        }
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
        step++;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawDDALine(50, 50, 400, 50, Solid); // Solido
    drawDDALine(50, 100, 400, 100, Dotted); // Puntos
    drawDDALine(50, 150, 400, 150, Dashed); // Guiones
    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);  // Cambiado a blanco
    glColor3f(0.0, 0.0, 0.0);  // Color de las líneas, en este caso negro
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 500.0, 0.0, 500.0);
}

int main_01(int &argc, char** &argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("DDA");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

/* ------------------------------------------------------------------------------------------------ */

struct casa {
    int x, y;         // Posición de la esquina inferior izquierda
    int altura, ancho; // Altura y anchura de la casa
};

struct carro {
    int x, y; // Posición de la esquina inferior izquierda del carro
    int largo, alto; // Dimensiones del carro
};

void dibujarCasa(casa &C) {
    // Parte rectangular (casa)
    glColor3f(0.0, 0.0, 1.0); // color-azul
    glBegin(GL_QUADS);
    glVertex2i(C.x, C.y);
    glVertex2i(C.x + C.ancho, C.y);
    glVertex2i(C.x + C.ancho, C.y + C.altura);
    glVertex2i(C.x, C.y + C.altura);
    glEnd();
    
    cout << "Altura: " << C.altura << endl;
    cout << "Ancho: " << C.ancho << endl;

    // Parte triangular (casa)
    glColor3f(1.0, 0.0, 0.0); // color-rojo
    glBegin(GL_TRIANGLES);
    glVertex2i(C.x, C.y + C.altura);
    glVertex2i(C.x + C.ancho, C.y + C.altura);
    glVertex2i(C.x + C.ancho / 2, C.y + C.altura + C.altura / 2); // Vértice del tejado
    glEnd();
}

void dibujarCirculo(int cx, int cy, int r) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.14159265358979323846 / 180.0; // Convertir ángulo a radianes
        int x = cx + r * cos(theta); // Calcular la coordenada x
        int y = cy + r * sin(theta); // Calcular la coordenada y
        glVertex2i(x, y);
    }
    glEnd();
}

void dibujarCarro(carro &C) {
    // Color del carro (rojo)
    glColor3f(1.0, 0.0, 0.0);

    // Cuerpo del carro
    glBegin(GL_QUADS);
    glVertex2i(C.x, C.y);
    glVertex2i(C.x + C.largo, C.y);
    glVertex2i(C.x + C.largo, C.y + C.alto);
    glVertex2i(C.x, C.y + C.alto);
    glEnd();

    // Ruedas del carro (negro)
    glColor3f(0.0, 0.0, 0.0);

    // Rueda izquierda
    dibujarCirculo(C.x + C.largo / 6, C.y - C.alto / 4, C.alto / 4);

    // Rueda derecha
    dibujarCirculo(C.x + 5 * C.largo / 6, C.y - C.alto / 4, C.alto / 4);
}


void trasladarCasa(casa &C, int nuevoX, int nuevoY) {
    C.x = nuevoX;
    C.y = nuevoY;
}

void escalarCasa(casa &C, float factor) {
    // Calcular el nuevo ancho y altura
    int nuevoAncho = C.ancho * factor;
    int nuevaAltura = C.altura * factor;

    // Reajustar x, y para mantener el centro en la misma posición relativa
    C.x -= (nuevoAncho - C.ancho) / 2;
    C.y -= (nuevaAltura - C.altura) / 2;

    // Aplicar las nuevas dimensiones
    C.ancho = nuevoAncho;
    C.altura = nuevaAltura;
}

void display2(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    casa C;
    C.x = 100; C.y = 100; C.altura = 100; C.ancho = 200; // Inicializar la casa
    
    carro Car;
    Car.x = 300; Car.y = 80; Car.largo = 120; Car.alto = 60; // Initialize the car
    
    // dibujarCarro(Car);
    
    // dibujarCasa(C);
    
    //Trasladar la casa a una nueva posición
    //trasladarCasa(C, 200, 200); // Nuevas coordenadas para la casa
    
    escalarCasa(C, 1.5);
    dibujarCasa(C);
    
    
    
    glFlush(); // Procesar todas las rutinas OpenGL
}

int main_02(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("CASA");
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 500.0, 0.0, 500.0);
    glutDisplayFunc(display2);
    glutMainLoop();

    return 0;
}

/* ------------------------------------------------------------------------------------------------ */


int main(int argc, char** argv){
    
    // main_01(argc, argv); // lineas
    
    main_02(argc, argv);
    
    return 0;
}

