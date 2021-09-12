#include "OpenT12.h"

//https://github.com/ArduboyCollection/arduboy-moire

const int screenWidth = 128;
const int screenHeight = 64;

const int numberOfLines = 20;

const int maxSpeed = 6;

/*
 * structs that describe the scene
 */

struct Point
{
    int x;
    int y;
};

struct Velocity
{
    int dx;
    int dy;
};

struct End
{
    Point p;
    Velocity v;
};

struct Line
{
    End e0;
    End e1;
};

struct Scene
{
    Line lines[numberOfLines];
    int indexOfHeadLine = numberOfLines - 1;
};

/*
 * scene initialization
 */

static Point randomPoint()
{
    return {random(screenWidth), random(screenHeight)};
}

static int randomSpeed()
{
    return random(1, maxSpeed);
}

static Velocity randomVelocity()
{
    return {randomSpeed(), randomSpeed()};
}

static End randomEnd()
{
    return {randomPoint(), randomVelocity()};
}

static Line randomLine()
{
    return {randomEnd(), randomEnd()};
}

static Point PointZero = {0, 0};
static Velocity VelocityZero = {0, 0};
static End EndZero = {PointZero, VelocityZero};
static Line LineZero = {EndZero, EndZero};


/*
 * functions to advance basic value types
 */
static End endByAdvancingEnd(End en)
{

    int ndx = en.v.dx;
    int ndy = en.v.dy;

    int nx = en.p.x + ndx;
    int ny = en.p.y + ndy;

    if (nx < 0 || nx > screenWidth)
    {
        ndx = -1 * sign(ndx) * randomSpeed();
        nx = en.p.x + ndx;
    }
    if (ny < 0 || ny > screenHeight)
    {
        ndy = -1 * sign(ndy) * randomSpeed();
        ny = en.p.y + ndy;
    }

    return {
        {nx,  ny},
        {ndx, ndy}
    };
}

static Line lineByAdvancingLine(Line line)
{
    return {
        endByAdvancingEnd(line.e0),
        endByAdvancingEnd(line.e1)
    };
}

/*
 * functions to operate on pointer to scene
 */

static void sceneInit(Scene* scene)
{
    for (int i = 0; i < numberOfLines; i++)
    {
        scene->lines[i] = LineZero;
    }
    scene->lines[scene->indexOfHeadLine] = randomLine();
}

static Line* sceneLine(Scene* scene, int index)
{
    int j = (scene->indexOfHeadLine + 1 + index) % numberOfLines;
    return &(scene->lines[j]);
}

static void sceneAdvance(Scene* scene)
{
    Line head = scene->lines[scene->indexOfHeadLine];
    Line nextHead = lineByAdvancingLine(head);
    scene->indexOfHeadLine = (scene->indexOfHeadLine + 1) % numberOfLines;
    scene->lines[scene->indexOfHeadLine] = nextHead;
}


static void drawLine(Line* line, uint16_t color)
{
    Disp.drawLine(line->e0.p.x, line->e0.p.y, line->e1.p.x, line->e1.p.y);
}


void RunSleepLoop(void)
{
    static Scene scene;

    static bool SleepScreenInit = false;
    if (!SleepScreenInit)
    {
        sceneInit(&scene);
        SleepScreenInit = true;
    }

    if (!SleepEvent) return;

    Line* tail = sceneLine(&scene, 0);
    drawLine(tail, 0);
    sceneAdvance(&scene);
    for (int i = 0; i < numberOfLines; i++)
    {
        Line* line = sceneLine(&scene, i);
        drawLine(line, 1);
    }
} 



