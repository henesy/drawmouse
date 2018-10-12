#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

#define width 49
#define b1 1
#define b2 2
#define b3 4

void updatemouse(void);
void initmouse(void);
void eresized(int);

Image *mouse;

Point p;

/* Menus */
char *buttons[] = {"exit", 0};
Menu menu = { buttons };

/* Radius of mouse */
int r = 120;
int borderWidth = 3;


/* Draws an image of a mouse and shows clicks */
void
main(int argc, char *argv[])
{
	USED(argc, argv);

	Event ev;
	int e, timer;

	/* Initiate graphics and mouse */
	if(initdraw(nil, nil, "bouncing mouse demo") < 0)
		sysfatal("initdraw failed: %r");

	einit(Emouse);

	/* Start our timer
	 * move the mouse every 5 milliseconds
	 * unless there is an Emouse event */

	timer = etimer(0, 5);

	/* Simulate a resize event to draw the background
	 * and acquire the screen dimensions */

	eresized(0);

	initmouse();

	/* Main event loop */

	int fd = open("/dev/mouse", OREAD);
	char last = 'x';
	for(;;){
		// see: mouse(3)
		char buf[width];
		read(fd, buf, width);
		char button = buf[35];
		if(button != last){
			switch(button){
			case '0'+b1:
				print("\Left button pressed.\n");
				break;
			case '0'+b2:
				print("Middle button pressed.\n");
				break;
			case '0'+b3:
				print("Right button pressed.\n");
				break;
			case '0'+(b1|b2):
				print("Left and Middle buttons pressed.\n");
				break;
			case '0'+(b1|b3):
				print("Left and Right buttons pressed.\n");
				break;
			case '0'+(b2|b3):
				print("Middle and Right buttons pressed.\n");
				break;
			case '0'+(b1|b2|b3):
				print("All buttons pressed.\n");
				break;
			case '0':
				print("No buttons pressed.\n");
			}
			last = button;
		}
		
		
		/* If there is a mouse event, the rightmost button
		 * pressed and the first menu option selected
		 * then exit.. */

		e = event(&ev);

		if( (e == Emouse) &&
			(ev.mouse.buttons & 4) && 
			(emenuhit(3, &ev.mouse, &menu) == 0)) exits(nil);
		else 
			if(e == timer)
				updatemouse();
	}
}


/* Change direction of mouse if collision
 * else move the mouse and draw to screen
 * The new mouse image draws over the previous image
 * so there is no need to redraw the whole screen */

void
updatemouse()
{
	// Set position static
	static Point bp={0,0};
	bp.x = (p.x / 2) - (r*2);
	bp.y = (p.y / 2) - (r*2);
	draw(screen, rectaddpt(screen->r, bp), mouse, nil, ZP);
}



/* Graphics library requires this function */

void
eresized(int new)
{
	if(new && getwindow(display, Refnone) < 0)
		sysfatal("can't reattach to window");

	/* Store new screen coordinates for collision detection */
	p = Pt(Dx(screen->r), Dy(screen->r));

	/* Draw the background DWhite */
	draw(screen, insetrect(screen->r, 4), 
			allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DWhite), 
			nil, ZP);
}


/* Draw red mouse inside a square of white background
 * When mouse is drawn at new position, background will
 * blot out the previous image */

void
initmouse()
{
	Image *brush;
	brush=allocimage(display, Rect(0,0,1,1), CMAP8, 1, DBlack);
	mouse=allocimage(display, (Rectangle){(Point){0,0},(Point){r*4,r*4}},
			screen->chan, 0, DWhite);
	ellipse(mouse, (Point){r*2,r*2}, r, r, borderWidth, brush, ZP);
}
