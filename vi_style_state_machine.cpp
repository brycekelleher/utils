#include <stdio.h>

// ===================================================
// State input processing code

static char currentinput;

char cmd_getinput()
{
	return currentinput;
}

static void cmd_none()
{}

static void cmd_cycle_rendermode()
{
	printf("cycle rendermode\n");
	//viewModes = (viewmode_t)((viewModes + 1) % NUM_RENDER_MODES);
}

static void cmd_render_wireframe()
{
	printf("wireframe rendering\n");
	//viewModes = RENDER_WIREFRAME;
}

static void cmd_render_filled()
{
	printf("filled rendering\n");
	//viewModes = RENDER_FILLED_WIREFRAME;
}

static void cmd_render_colors()
{
	printf("vertex color rendering\n");
	//viewModes = RENDER_RGB;
}

static void cmd_cull_front()
{
	//cullMode = CULL_FRONT;
}

static void cmd_cull_back()
{
	//cullMode = CULL_BACK;
}

static void cmd_cull_none()
{
	//cullMode = CULL_NONE;
}

static void cmd_cycle_cull_mode()
{
	//cullMode = (cullmode_t)((cullMode + 1) % NUM_CULL_MODES);
}

static void cmd_escape()
{
	printf("escape was pressed in S_CMD\n");
}

// viewstates a-z
//static viewstate_t viewstatebuf[26];

static void cmd_mark_view()
{
	int index = cmd_getinput() - 'a';
	//viewstatebuf[index] = viewstate;
}

static void cmd_recall_view()
{
	int index = cmd_getinput() - 'a';
	//viewstate = viewstatebuf[index];
}

static char ranges[2][128];
static char* range;

static void cmd_range_1_first_number()
{
	range = ranges[0];
	*range++ = cmd_getinput();

	//printf("got first number of range 1\n");
}

static void cmd_range_1_other_number()
{
	if(range - ranges[0] >= 128)
		return;

	*range++ = cmd_getinput();

	//printf("got other number of range 1\n");
}

static void cmd_range_2_first_number()
{
	// tie off range[0]
	*range = 0;

	// switch to range[1]
	range = ranges[1];
	*range++ = cmd_getinput();

	//printf("got first number of range 2\n");
}

static void cmd_range_2_other_number()
{
	if(range - ranges[1] >= 128)
		return;

	*range++ = cmd_getinput();

	//printf("got other number of range 2\n");
}

static void cmd_end_range()
{
	// tie off range[1]
	*range = 0;

	// print ranges
	printf("ranges: %s, %s\n", ranges[0], ranges[1]);
}

enum state_t 
{ 
	S_CMD,
	S_RENDER_MODE,
	S_CULL_MODE,
	S_END, 
	S_MARK_VIEW_1,
	S_RECALL_VIEW_1,
	S_RANGE_1, S_RANGE_2, S_RANGE_3, S_RANGE_4 
};

//state, input, action, nextstate
typedef struct
{
	state_t		state;
	const char*	inputstring;
	void		(*action)();
	state_t		nextstate;

} state_desc_t;

// currentstate, input, action, next_state
state_desc_t statetab[] = 
{
	// main vi machine	
	{ S_CMD,			"r", cmd_none, S_RENDER_MODE },
	{ S_CMD,			"c", cmd_none, S_CULL_MODE},
	{ S_CMD,			"R", cmd_cycle_rendermode, S_CMD },
	{ S_CMD,			"C", cmd_cycle_cull_mode, S_CMD },
	{ S_CMD,			"\x1n", cmd_escape, S_CMD },
	{ S_CMD,			"m", cmd_none, S_MARK_VIEW_1 },
	{ S_CMD,			"'", cmd_none, S_RECALL_VIEW_1 },
	{ S_RENDER_MODE,	"r", cmd_cycle_rendermode, S_CMD },
	{ S_RENDER_MODE,	"w", cmd_render_wireframe, S_CMD },
	{ S_RENDER_MODE,	"f", cmd_render_filled, S_CMD },
	{ S_RENDER_MODE,	"c", cmd_render_colors, S_CMD },
	{ S_CULL_MODE,		"f", cmd_cull_front, S_CMD },
	{ S_CULL_MODE,		"b", cmd_cull_back, S_CMD },
	{ S_CULL_MODE,		"n", cmd_cull_none, S_CMD },
	{ S_CULL_MODE,		"c", cmd_cycle_cull_mode, S_CMD },
	{ S_MARK_VIEW_1,	"abcdefghijklmnopqrstuvwxyz", cmd_mark_view, S_CMD },
	{ S_RECALL_VIEW_1,	"abcdefghijklmnopqrstuvwxyz", cmd_recall_view, S_CMD},

	// states to lex a range
	// how to handle acceptance state?
	//abcdefghijklmnopqrstuvwxyz
	//ABCDEFGHIJKLMNOPQRSTUVWXYZ
	//0123456789
	{ S_RANGE_1,		"0123456789", cmd_range_1_first_number, S_RANGE_2 }, // parse out the first number
	{ S_RANGE_1,		NULL, cmd_none, S_RANGE_1},
	{ S_RANGE_2,		"0123456789", cmd_range_1_other_number, S_RANGE_2 }, // parse out any other numbers
	{ S_RANGE_2,		",", cmd_none, S_RANGE_3 },
	{ S_RANGE_2,		NULL, cmd_none, S_RANGE_1},
	{ S_RANGE_3,		"0123456789", cmd_range_2_first_number, S_RANGE_4 }, // parse out the first number
	{ S_RANGE_3,		NULL, cmd_none, S_RANGE_1},
	{ S_RANGE_4,		"0123456789", cmd_range_2_other_number, S_RANGE_4 }, // parse out any other numbers
	{ S_RANGE_4,		NULL, cmd_end_range, S_RANGE_1},	// should be end range?

	// end of state table
	{ S_END },
};

// state machine current state
static state_t current_state = S_CMD;

static bool ischarinstring(char c, const char *string)
{
	// a NULL string is a wildcard for any character
	if(!string)
		return true;

	for(; *string; string++)
	{
		if(*string == c)
			break;
	}

	return *string != 0;
}

state_desc_t *findstate(state_t state, char input)
{
	state_desc_t *desc = statetab;
	for(; desc->state != S_END; desc++)
	{
		if(desc->state == state && ischarinstring(input, desc->inputstring))
			return desc;
	}

	return NULL;
}

static void run_input(char input)
{
	currentinput = input;

	state_desc_t *desc = findstate(current_state, input);

	if(!desc)
	{
		// fixme: flush the full input buffer
		current_state = S_CMD;
		return;
	}

	// call the action
	desc->action();
	
	// set the next state
	current_state = desc->nextstate;
}

