// Demjen Nicole-Klarisza (Physics-Computer science)
// Simulation of the sirius binary star system

#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979f

typedef struct {
    int cx, cy; //the geometric center of the ellipse
    int rx, ry;//semi-major and semi-minor axes of the ellipse
    int color;
} Orbit;

//The parametric equations of the ellipse that were used:
// x=cx+rx*cos(t)
//y=cy+ry*sin(t)

void draw_ellipse(Orbit orb, float step)
{
    setcolor(orb.color);
    int W = getmaxx(); //returns the maximum width of the window
    int H = getmaxy();//return the maximum height of the window
    int x_prev = orb.cx + orb.rx;
    int y_prev = orb.cy;
    float t;
    for (t = step; t <= 2.0f*PI + step; t += step) {
        int xn = orb.cx + (int)(orb.rx * cosf(t));
        int yn = orb.cy + (int)(orb.ry * sinf(t));
        if (x_prev>=0&&x_prev<=W&&y_prev>=0&&y_prev<=H&&
            xn>=0&&xn<=W&&yn>=0&&yn<=H) //condition to no leave the window
            line(x_prev, y_prev, xn, yn);
        x_prev = xn; y_prev = yn;
    }
}

//the function finds a single specific point on the orbit, which corresponds to the current angle of the star (theta)
//the same equations have been used as previously
void position_on_orbit(Orbit orb, float theta, int *px, int *py)
{
    *px = orb.cx + (int)(orb.rx * cosf(theta));
    *py = orb.cy + (int)(orb.ry * sinf(theta));
}
//barycenter=center of mass around which the two stars orbit each other
void draw_barycenter(int bx, int by) 
{
    setcolor(YELLOW);
    circle(bx, by, 5); //bx,by barycenter coord.
    line(bx-9, by, bx+9, by);
    line(bx, by-9, bx, by+9);
}

//the stars have been colored in consideration with their actual color (given by their temperature)
void draw_star(int x, int y, int radius, int aura_radius, int star_color, int aura_color)
{
    setcolor(aura_color);
    setfillstyle(SOLID_FILL, aura_color);
    fillellipse(x, y, aura_radius, aura_radius);
    setcolor(star_color);
    setfillstyle(SOLID_FILL, star_color);
    fillellipse(x, y, radius, radius);
}


void show_data(float m1, float m2, float a, float e)
{
    char buf[80];
    int x = 750, y = 170;

    settextstyle(GOTHIC_FONT, HORIZ_DIR, 1);

    setcolor(WHITE);
    outtextxy(x, y, "System data:");

    setcolor(CYAN);
    sprintf(buf, "Sirius A:  %.3f M*", m1);
    outtextxy(x, y+32, buf);

    setcolor(MAGENTA);
    sprintf(buf, "Sirius B:  %.3f M*", m2);
    outtextxy(x, y+58, buf);

    setcolor(WHITE);
    sprintf(buf, "Semiaxis:   %.2f AU", a);
    outtextxy(x, y+84, buf);

    sprintf(buf, "Eccentricity:   %.4f", e);
    outtextxy(x, y+110, buf);

    float T = sqrtf((a*a*a)/(m1+m2));  //period of revolution
    sprintf(buf, "Period:  %.2f years", T);
    outtextxy(x, y+136, buf);

    sprintf(buf, "Periapsis: %.2f AU", a*(1.0f-e)); //the point in which the stars are the closest to each other, and where their velocity is max.
    outtextxy(x, y+162, buf);

    sprintf(buf, "Apoapsis:  %.2f AU", a*(1.0f+e)); //the point in which the stars are the furthest away from each other and are the slowest
    outtextxy(x, y+188, buf);

    setcolor(CYAN);
    outtextxy(x, y+224, "Sirius A Orbit");
    setcolor(MAGENTA);
    outtextxy(x, y+250, "Sirius B Orbit");
    setcolor(YELLOW);
    outtextxy(x, y+276, "Barycenter");
}


int main()
{ 
    //reading data from file
    FILE *f;
    float m1, m2, a, e;
    int read_ok = 0; //for the exception (in case the data from the file is not read)

    f = fopen("sirius.txt", "r");
    if (f) {
        //fscanf returns the number of values read successfully, so all 4 must be read
        int n = fscanf(f, "%f %f %f %f", &m1, &m2, &a, &e);
        fclose(f);
        if (n==4&&m1>0&&m2>0&&a>0&&e>=0&&e<1.0f)
            read_ok = 1;
    }
    if (!read_ok) {
        m1=2.063f; m2=1.018f; a=20.0f; e=0.5914f; //the data that should have been read is given
    }

    initwindow(1100, 750, "Binary Stars - Sirius A and B");
    delay(100);
    setbkcolor(BLACK);
    cleardevice();
     
     //1AU (one astronomical unit) corresponds to 18 pixels
    float scale = 18.0f;
    
    //semiaxis 
    float a1 = a * m2 / (m1 + m2);
    float a2 = a * m1 / (m1 + m2);
    
    //scaled semiaxis
    int a1_px = (int)(a1 * scale);
    int a2_px = (int)(a2 * scale);
    
    //c describes the offset from the ellipse's center 
    int c1_px = (int)(a1 * scale * e);
    int c2_px = (int)(a2 * scale * e);
    
    //coord. barycenter
    int bx = 270, by = 390;
    
    Orbit orbit_A, orbit_B;
    orbit_A.cx = bx - c1_px;
    orbit_A.cy = by;
    orbit_A.rx = a1_px;
    orbit_A.ry = (int)(a1_px * sqrtf(1.0f - e*e));
    orbit_A.color = CYAN;

    orbit_B.cx = bx + c2_px;
    orbit_B.cy = by;
    orbit_B.rx = a2_px;
    orbit_B.ry = (int)(a2_px * sqrtf(1.0f - e*e));
    orbit_B.color = MAGENTA;

    // static drawing
    setcolor(WHITE);
    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 4);
    outtextxy(120, 50, "~Binary star system- Sirius A & B~");

    //the orbits themselves are drawn inside the loop, with the same step every frame
    float orbit_step = 0.02f;

    setcolor(CYAN);    circle(orbit_A.cx, by, 3);
    setcolor(MAGENTA); circle(orbit_B.cx, by, 3);
    draw_barycenter(bx, by);

    show_data(m1, m2, a, e);

    settextstyle(GOTHIC_FONT, HORIZ_DIR, 1);
    setcolor(DARKGRAY);
    if (read_ok)
        outtextxy(20, 725, "sirius.txt");
    else
        outtextxy(20, 725, "Default_values (sirius.txt was not found)");
        
    
    float theta = 0.0f;
    float mean_step = 0.03f; //angle advanced per frame when r = a1_px

    int x_A_prev=0, y_A_prev=0, x_B_prev=0, y_B_prev=0;
    int first = 1;
    int paused = 0;

    while (1)
    {
        if (_kbhit()) {
            int key = getch();
            if (key == 27) break; // 27 is the ASCII code for ESC -> stops the loop
            if (key == 32) paused = !paused; // 32 is  space -> changes the value for the break
        }
        if (paused) { delay(50); continue; }

        // New positions
        int x_A_new, y_A_new, x_B_new, y_B_new;
        position_on_orbit(orbit_A, theta,      &x_A_new, &y_A_new);
        position_on_orbit(orbit_B, theta + PI, &x_B_new, &y_B_new);

        // The old positions get deleted
        if (!first) {
            draw_star(x_A_prev, y_A_prev, 11, 14, BLACK, BLACK);
            draw_star(x_B_prev, y_B_prev,  6,  8, BLACK, BLACK);
        }

        // The stars get drawn in their new positions
        draw_star(x_A_new, y_A_new, 9, 13, WHITE, LIGHTGRAY);
        draw_star(x_B_new, y_B_new, 4,  7, WHITE, CYAN);

        // The ellipse are re-drawn above the stars
        draw_ellipse(orbit_A, orbit_step);
        draw_ellipse(orbit_B, orbit_step);

        // The barycenter gets redrawn
        draw_barycenter(bx, by);

        if (first) first = 0;

        // Saves the old position
        x_A_prev=x_A_new; y_A_prev=y_A_new;
        x_B_prev=x_B_new; y_B_prev=y_B_new;

        // Variable velocity (Kepler's law)
        // Kepler's equation M = E - e*sin(E), with M = n*t, gives dE/dt = n/(1 - e*cos(E)).
        // Since r = a1*(1 - e*cos(E)), this is dE/dt = n*a1/r, so the step per frame
        // is proportional to 1/r (not 1/r^2: theta is the eccentric anomaly).
        float r_A = a1_px * (1.0f - e*cosf(theta));
        if (r_A < 1.0f) r_A = 1.0f;

        float total_step = mean_step * a1_px / r_A;
        if (total_step > 0.3f) total_step = 0.3f;  // maximum limit/frame

        theta += total_step;
        if (theta > 2.0f*PI) theta -= 2.0f*PI;

        delay(15);
    }

    closegraph();
    return 0;
}
