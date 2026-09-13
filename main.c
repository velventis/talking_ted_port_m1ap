#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <directfb/directfb.h>
#include <lgnc.h>
#include <time.h>
#include "action.h"

//lg default font path
#define FONT_PATH "/mnt/lg/res/lgfont/lg_display_dvb.ttf"
#define FONT_SIZE 28 

#define MAX_ACTIONS_VISIBLE 12
#define ANIMATION_FPS 24

// -- global app state --
static volatile int g_running = 1;
char *g_app_path = NULL;
static volatile int g_selected = 0;
static volatile int g_playing = 0;

static IDirectFB *g_pDfb = NULL;
static IDirectFBSurface *g_pPrimary = NULL;
static IDirectFBFont *g_pFont = NULL;

static void sleep_until(struct timespec *target) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    long sec = target->tv_sec - now.tv_sec;
    long nsec = target->tv_nsec - now.tv_nsec;

    if (nsec < 0) {
        sec--;
        nsec += 1000000000L;
    }
    if (sec > 0 || (sec == 0 && nsec > 0)) {
        struct timespec remaining = {
            .tv_sec = sec,
            .tv_nsec = nsec
        };
        nanosleep(&remaining, NULL);
    }
}

static int draw_image(const char *image_path, int x, int y) {
	IDirectFBImageProvider *provider = NULL;
	g_pDfb->CreateImageProvider(g_pDfb, image_path, &provider);
	
	DFBSurfaceDescription idesc;
	memset(&idesc, 0, sizeof(idesc));
	idesc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
	provider->GetSurfaceDescription(provider, &idesc);
	
	IDirectFBSurface *image = NULL;
	g_pDfb->CreateSurface(g_pDfb, &idesc, &image);
	
	//deocode and render image to surface
	provider->RenderTo(provider, image, NULL);
	g_pPrimary->Blit(g_pPrimary, image, NULL, x, y);
	g_pPrimary->Flip(g_pPrimary, NULL, 0);

	image->Release(image);
	provider->Release(provider);
	return 0;
}

static void run_action(int act) {
	printf("act[%i]	name=%s\n", act, g_actions[act].name);
	printf("		sound_path=%s\n", g_actions[act].sound_path);
	printf("		frames_path=%s\n", g_actions[act].frames_path);
	printf("		frame_count=%i\n", g_actions[act].frame_count);
	
	g_playing = 1;
	//play sound
	char sound_path[MAX_PATH];
	snprintf(sound_path, sizeof(sound_path), "%s/assets/%s", g_app_path, g_actions[act].sound_path);
	LGNC_MEDIA_PlayFile(CH_MAIN, sound_path, 1);
	
	//animation timing
	struct timespec next_frame;
	clock_gettime(CLOCK_MONOTONIC, &next_frame);
	const long frame_ns = 1000000000L / ANIMATION_FPS;
	
	for (int i = 1; i < g_actions[act].frame_count; i++) {
		char frame_path[MAX_PATH];
		snprintf(frame_path, sizeof(frame_path), "%s/assets/%s/%d.jpg", g_app_path, g_actions[act].frames_path, i);
		//printf("%i %s\n", i, frame_path);
		draw_image(frame_path, 400, 0);
		
		next_frame.tv_nsec += frame_ns;
		if (next_frame.tv_nsec >= 1000000000L) {
			next_frame.tv_sec++;
			next_frame.tv_nsec -= 1000000000L;
		}
		sleep_until(&next_frame);
	}
	LGNC_MEDIA_Stop(CH_MAIN);
	g_playing = 0;
}

//assumed icon should be 80x80
static void draw_action_list(void) {
	//bg clear
	g_pPrimary->SetColor(g_pPrimary, 0, 0, 0, 255);
	g_pPrimary->FillRectangle(g_pPrimary, 0, 620, 1280, 80);
	g_pPrimary->Flip(g_pPrimary, NULL, 0);
	
	int x = 23;
	//for page
	int p_start = (g_selected / MAX_ACTIONS_VISIBLE) * MAX_ACTIONS_VISIBLE;
	int p_end = p_start + MAX_ACTIONS_VISIBLE;
	if (p_end > NUM_ACTIONS) {
        p_end = NUM_ACTIONS;
	}
	
	for (int i = p_start; i < p_end; i++) {
		//printf("action %i, name=%s\n", i, g_actions[i].name);
		
		char icon_path[MAX_PATH];
		snprintf(icon_path, sizeof(icon_path), "%s/assets/%s", g_app_path, g_actions[i].icon_path);
		//printf("	icon_path=%s\n", icon_path);
		
		draw_image(icon_path, x, 620);
		x += 105;
	}
}

static void draw_selection_box(void) {
	//printf("g_selected=%i\n", g_selected);
	//bg clear
	g_pPrimary->SetColor(g_pPrimary, 0, 0, 0, 255);
	g_pPrimary->FillRectangle(g_pPrimary, 0, 700, 1280, 20);
	
	//for page
	int p_start = (g_selected / MAX_ACTIONS_VISIBLE) * MAX_ACTIONS_VISIBLE;
	int p_idx = g_selected - p_start;
	int x = 23 + (p_idx * 105) - 5;
	
	//selector		
	g_pPrimary->SetColor(g_pPrimary, 255, 0, 0, 255);
	g_pPrimary->FillRectangle(g_pPrimary, x, 700, 80, 20);
	
	g_pPrimary->Flip(g_pPrimary, NULL, 0);
}

static BOOLEAN MyKeyHandler(UINT32 keyCode, LGNC_KEY_COND_T cond, LGNC_ADDITIONAL_INPUT_INFO_T *extra) {
	//printf(">>> KEY 0x%x cond=%d\n", keyCode, cond);
    if (cond != 0) {
        return 1;
    }
	
	//no action when animation is playing
	if (g_playing) {
		return 1;
	}

    int p_sel = g_selected;
    int p_page = (p_sel / MAX_ACTIONS_VISIBLE) * MAX_ACTIONS_VISIBLE;

    if (keyCode == KEY_RIGHT) {
        if (g_selected >= NUM_ACTIONS - 1) {
            g_selected = 0;
		} else {
            g_selected+=1;
		}
		
    } else if (keyCode == KEY_LEFT) {
        if (g_selected <= 0) {
            g_selected = NUM_ACTIONS - 1;
		} else {
            g_selected-=1;
		}
		
	} else if (keyCode == KEY_OK) {
		//printf("start action %i\n", g_selected);
		run_action(g_selected);
		
    } else {
        return 1;
    }

	if (keyCode == KEY_LEFT || keyCode == KEY_RIGHT) {
		int n_page = (g_selected / MAX_ACTIONS_VISIBLE) * MAX_ACTIONS_VISIBLE;
		//printf("sel:%d -> %d, page: %d -> %d\n", p_sel, g_selected, p_page, n_page);

		if (p_page != n_page) {
			draw_action_list();
		}
		draw_selection_box();
	}

    return 1;
}

static LGNC_STATUS_T MyMsgHandler(int msgType, UINT32 a, UINT8 *b, UINT16 c) {
	if (msgType == LGNC_MSG_TERMINATE) {
		g_running = 0;
	}
	return LGNC_HANDLED;
}

int main(int argc, char **argv) {
	printf("start !\n");
	
	if (argc < 2) {
		printf("invalid args\n");
		return 1;
	}
	
	g_app_path = argv[1];
	printf("g_app_path=%s\n", g_app_path);

	//initalize LGNC
	LGNC_SYSTEM_CALLBACKS_T cb;
	memset(&cb, 0, sizeof(cb));
	cb.pfnMsgHandler       = MyMsgHandler;
	cb.pfnKeyEventCallback = MyKeyHandler;
	LGNC_STATUS_T lstat = LGNC_SYSTEM_Initialize(argc, argv, &cb);
	printf("LGNC_SYSTEM_Initialize returned %d\n", lstat);

	//initialize directfb
	int dfargc = 2;
	char *dfargv[] = { (char*)"myapp", (char*)"--dfb:no-sighandler", NULL };
	char **pDfArgv = dfargv;
	DirectFBInit(&dfargc, &pDfArgv);

	DirectFBCreate(&g_pDfb);
	g_pDfb->SetCooperativeLevel(g_pDfb, DFSCL_FULLSCREEN);

	//create primiary surface
	DFBSurfaceDescription desc;
	memset(&desc, 0, sizeof(desc));
	desc.flags = DSDESC_CAPS;
	desc.caps  = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	g_pDfb->CreateSurface(g_pDfb, &desc, &g_pPrimary);

	//load font
	DFBFontDescription fdesc;
	memset(&fdesc, 0, sizeof(fdesc));
	fdesc.flags  = DFDESC_HEIGHT;
	fdesc.height = FONT_SIZE;
	g_pDfb->CreateFont(g_pDfb, FONT_PATH, &fdesc, &g_pFont);

	//draw background
	g_pPrimary->SetColor(g_pPrimary, 0, 0, 0, 255);
	g_pPrimary->FillRectangle(g_pPrimary, 0, 0, 1280, 720);
	g_pPrimary->SetFont(g_pPrimary, g_pFont);
	
	//title
	g_pPrimary->SetColor(g_pPrimary, 255, 255, 255, 255);
	g_pPrimary->DrawString(g_pPrimary, "Hello world", -1, 60, 50, DSTF_LEFT);

	draw_action_list();
	draw_selection_box();

	printf("running...\n");
	while (g_running) {
		sleep(1);	//sleep main thread infinitely
	}
		
	printf("exit...\n");
	//deinitialize everything to exit cleanly
	if (g_pFont) g_pFont->Release(g_pFont);
	if (g_pPrimary) g_pPrimary->Release(g_pPrimary);
	if (g_pDfb) g_pDfb->Release(g_pDfb);

	LGNC_SYSTEM_Finalize();
		
	return 0;
}