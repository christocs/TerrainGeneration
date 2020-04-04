#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdio>

#include "TerrainHeightMap.hpp"
#include <iostream>

void init(int width, int height)
{
    //Set the viewport
    glViewport( 0.f, 0.f, 800, 400 );

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    //glOrtho( 0.0, width, height, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );
}

void initOrtho(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width / 2, width / 2, -height / 2, height / 2, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

auto main(int argc, char **argv) -> int {
    (void)argc;
    (void)argv;

    auto isRunning = true;
    auto width     = 800;
    auto height    = 600;

    auto sdl    = SDL_version{};

    printf("SDL2 version:\t\t%d.%d.%d\n", sdl.major, sdl.minor, sdl.patch);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    auto *window  = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    auto *context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, context);

    // turn on vsync
    SDL_GL_SetSwapInterval(1);

    init(width, height);   

    /*
    gluLookAt(0, 0, -10,
                0, 0, 0,
                0, 1, 0);
                */
    
//glEnable(GL_DEPTH_TEST);
    auto t = TerrainHeightMap();
    t.load("assets/height128.raw", 128, 128);

    int xPos = 0;
    int xDiff = 1;
    int yPos = 0;
    int yDiff = 1;
    int zPos = 0;
    int zDiff = 1;
    int angle = 1;
    while (isRunning) {
        auto event = SDL_Event{};

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    std::cout << SDL_GetKeyName( event.key.keysym.sym ) << std::endl;
                    if (*SDL_GetKeyName(event.key.keysym.sym) == 'A') {
                        xPos -= xDiff;
                    } else if (*SDL_GetKeyName(event.key.keysym.sym) == 'D') {
                        xPos += xDiff;
                    } else if (*SDL_GetKeyName(event.key.keysym.sym) == 'S') {
                        zPos -= zDiff;
                    } else if (*SDL_GetKeyName(event.key.keysym.sym) == 'W') {
                        zPos += zDiff;
                    }
                    std::cout << "x: " << xPos << ", y: " << yPos << ", z: " << zPos << std::endl;

                    break;
                default:
                    break;
            }
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT);

        glTranslatef(xPos, yPos, zPos);
    /*
        gluLookAt(xPos, yPos, zPos,
                0, 0, 0,
                0, 1, 0);
                */
                
        
        //glRotatef(0, 0, 1, 1);
        //glRotatef(angle++, 0, 1, 0);
        //t.render();

        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        
        glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 3);
        glVertex3f(0, 150, 0);
        glColor3f(0, 1, 0);
        glVertex3f(-150, 0, 0);
        glColor3f(0, 0, 1);
        glVertex3f(150, 0, 0);
        glEnd();
        
       /*
        glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 150, 0);
        glColor3f(0, 1, 0);
        glVertex3f(-150, 0, 0);
        glColor3f(0, 0, 1);
        glVertex3f(150, 0, 0);
        glEnd();
        */
        

        SDL_GetWindowSize(window, &width, &height);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
