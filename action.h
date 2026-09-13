#define MAX_NAME 32
#define MAX_PATH 512

typedef struct {
	char name[MAX_NAME];
	char icon_path[MAX_PATH];
	char sound_path[MAX_PATH];
	char frames_path[MAX_PATH];
	unsigned int frame_count;
} action;

//actions
static action g_actions[] = {
	//smoke
	{ "smoking_cigarette", 	"img/btn_smoke_cigarette1.png", "sound/ted_smoking_cigarette.mp3", 	"frames/ted_smoking_cigarette", 97 },
	{ "smoking_bond", 		"img/btn_bottle_1.png", 		"sound/ted_smoking_bond.mp3", 		"frames/ted_smoking_bond", 		157 },
	{ "smoking_joint", 		"img/btn_joint_1.png", 			"sound/ted_smoking_bond.mp3", 		"frames/ted_smoking_bond", 		133 },
	{ "glass_pipe", 		"img/btn_pipe_1.png", 			"sound/ted_glass_pipe.mp3", 		"frames/ted_glass_pipe", 		97 },
	
	//drink
	{ "whiskey", 			"img/btn_whiskey_1.png", 		"sound/ted_whiskey.mp3", 			"frames/ted_whiskey", 			79 },
	{ "martini", 			"img/btn_martini_1.png", 		"sound/ted_martini.mp3", 			"frames/ted_martini", 			145 },
	{ "drinking_beer", 		"img/btn_drink_beer1.png", 		"sound/ted_drinking_beer.mp3", 		"frames/ted_drinking_beer", 	146 },
	{ "humping", 			"img/btn_drink_fart1.png", 		"sound/ted_humping.mp3", 			"frames/ted_humping", 			145 },
	
	//speak
	{ "talk_thunder", 		"img/btn_thunder_1.png", 		"sound/ted_talk_thunder.mp3", 		"frames/ted_talk_thunder", 		51 },
	{ "talk_acting", 		"img/btn_acting_1.png",			"sound/ted_talk_acting.mp3", 		"frames/ted_talk_acting", 		42 },
	{ "talk_dirtyfuzzy",	"img/btn_fuzzy_1.png", 			"sound/ted_talk_dirtyfuzzy.mp3", 	"frames/ted_talk_dirtyfuzzy", 	91 },
	{ "talk_vodka", 		"img/btn_vodka_1.png", 			"sound/ted_talk_vodka.mp3", 		"frames/ted_talk_vodka", 		67 },
	{ "talk_fucking",		"img/btn_fucking_1.png", 		"sound/ted_talk_fucking.mp3", 		"frames/ted_talk_fucking", 		36 },
	{ "talk_iloveyou", 		"img/btn_talk_heart1.png", 		"sound/ted_talk_iloveyou.mp3", 		"frames/ted_talk_iloveyou", 	35 },
	{ "talk_draw", 			"img/btn_talk_draw1.png", 		"sound/ted_talk_draw.mp3", 			"frames/ted_talk_draw", 		55 },
	{ "talk_onemoretime",	"img/btn_talk_speak1.png", 		"sound/ted_talk_onemoretime.mp3", 	"frames/ted_talk_onemoretime", 	34 },
	{ "talk_comehere", 		"img/btn_talk_finger1.png", 	"sound/ted_talk_comehere.mp3", 		"frames/ted_talk_comehere", 	39 },
	{ "talk_email", 		"img/btn_talk_mail1.png", 		"sound/ted_talk_email.mp3", 		"frames/ted_talk_email", 		27 },
};
#define NUM_ACTIONS (sizeof(g_actions) / sizeof(g_actions[0]))