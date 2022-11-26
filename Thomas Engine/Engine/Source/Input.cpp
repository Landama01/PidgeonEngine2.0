#include "Application.h"

//Modules
#include "Input.h"
#include "Window.h"
#include "Editor.h"

#include "Globals.h"

#include "FileSystem.h"
#include "ResourceManager.h"
#include "Renderer3D.h"

#define MAX_KEYS 300

Input::Input(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Input";

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
}

// Called before render is available
bool Input::Init()
{
	LOG(LogType::L_NORMAL, "Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG(LogType::L_ERROR, "SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status Input::PreUpdate(float dt)
{
	SDL_PumpEvents();
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
			{
				keyboard[i] = KEY_DOWN;
				LogInputEvent(i, KEY_DOWN);
			}				
			else if (keyboard[i] != KEY_REPEAT)
			{
				keyboard[i] = KEY_REPEAT;
				LogInputEvent(i, KEY_REPEAT);
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
			{
				keyboard[i] = KEY_UP;
				LogInputEvent(i, KEY_UP);
			}
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	wheel = 0;

	for(int i = 0; i < MAX_MOUSE_BUTTONS; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
			{
				mouse_buttons[i] = KEY_DOWN;
				LogInputEvent(1000 + i, KEY_DOWN);
				LogInputEvent(1000 + i, KEY_REPEAT);
			}
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN) ? 
				mouse_buttons[i] = KEY_UP,LogInputEvent(1000 + i, KEY_UP) : mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	update_status ret = UPDATE_CONTINUE;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			wheel = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case (SDL_DROPFILE):
			app->resourceManager->ImportFile(FileSystem::ExtractLocalDiskForward(e.drop.file).c_str());
			SDL_free(e.drop.file);  
			break;
			
			case SDL_QUIT:
				quit = true;
			break;

			case SDL_WINDOWEVENT:
				App->window->ManageEvent(&e);
				break;
			break;
		}
	}

	if(keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		ret = UPDATE_STOP;

	return ret;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void Input::OnGUI()
{
	FileSystem::OnGui();

	if (ImGui::CollapsingHeader("Input"))
	{
		IMGUI_PRINT("Mouse Position: ", "%i,%i", mouse_x, mouse_y);
		IMGUI_PRINT("Mouse Motion: ", "%i,%i", mouse_x_motion, mouse_x_motion);
		IMGUI_PRINT("Mouse Wheel: ", "%i", wheel);

		ImGui::Separator();

		ImGui::BeginChild("Input Log");
		ImGui::TextUnformatted(inputBuf.begin());
		if (need_scroll)
			ImGui::SetScrollHere(1.0f);
		need_scroll = false;
		ImGui::EndChild();
	}
}

void Input::AddInput(const char* entry)
{
	inputBuf.appendf(entry);
	need_scroll = true;
}

void Input::LogInputEvent(uint key, uint state)
{
	static char entry[512];
	static const char* states[] = { "IDLE", "DOWN", "REPEAT", "UP" };

	if (key < 1000)
		sprintf_s(entry, 512, "Keybr: %02u - %s\n", key, states[state]);
	else
		sprintf_s(entry, 512, "Mouse: %02u - %s\n", key - 1000, states[state]);
	AddInput(entry);	
}
