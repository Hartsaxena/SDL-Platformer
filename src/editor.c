#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "calc.h"
#include "debug.h"
#include "editor.h"
#include "front.h"
#include "obj.h"
#include "parse.h"
#include "player.h"
#include "render.h"
#include "weather.h"


int RemoveDirectory(const char *path) {
    /*
    This function deletes a directory, including all of its contents.
    I copy-pasted this function from Stack Overflow, so I'm not sure how it works.
    */
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p=readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2; 
            buf = malloc(len);

            if (buf) {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = RemoveDirectory(buf);
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}


void editor_DeleteNEWFolder()
{
    /*
    This function deletes the NEW folder.
    */
    if (DEBUG_MODE)
        printf("Deleting NEW folder... ");
    char* Path = "maps\\NEW";
    RemoveDirectory(Path);
    if (DEBUG_MODE)
        printf("Success!\n");
}


static editor_Selection* Selection = NULL;
static bool AwaitChord = false;
static int PreviousSaveTime = 0;
extern int* render_CameraX;
extern int* render_CameraY;
void editor_Update(player_Player* Player, bool InputKeys[286], front_MouseState* MouseState, obj_Barrier* BarriersHead, obj_Entity* EntitiesHead, weather_Weather* WeatherInstance)
{
    if (Selection == NULL) {
        Selection = malloc(sizeof(editor_Selection));
    }

    int* GlobalMousePosition = render_GetGlobalMousePosition(MouseState->x, MouseState->y);
    int GlobalMouseX = GlobalMousePosition[0];
    int GlobalMouseY = GlobalMousePosition[1];

    if (InputKeys[SDL_SCANCODE_LCTRL] && InputKeys[SDL_SCANCODE_S] && !AwaitChord) {
        // Only allow save every once in a while.
        if (time(0) - PreviousSaveTime > EDITOR_SAVE_DELAY_MS / 1000) {
            if (DEBUG_MODE)
                printf("CTRL S pressed. Saving level.\n");
            editor_DeleteNEWFolder();
            parse_SaveMap(BarriersHead, EntitiesHead, WeatherInstance, "maps\\NEW");
            if (DEBUG_MODE)
                printf("Level saved.\n");
            PreviousSaveTime = time(0);
        }
    }

    if (InputKeys[SDL_SCANCODE_LCTRL] && InputKeys[SDL_SCANCODE_N] && !AwaitChord) {
        if (DEBUG_MODE)
            printf("CTRL N pressed. Awaiting Chord...\n");
        AwaitChord = true;
    } else if (AwaitChord) {
        // Detecting Chord

        bool W = InputKeys[SDL_SCANCODE_W]; // Wall barrier
        bool P = InputKeys[SDL_SCANCODE_P]; // Platform barrier
        bool V = InputKeys[SDL_SCANCODE_V]; // Void barrier

        if (W || P || V) {
            AwaitChord = false;
            // Create new Barrier
            if (DEBUG_MODE)
                printf("Barrier Chord detected. Creating new barrier of type %s.\n", W ? "Wall" : P ? "Platform" : "Void");
            obj_Barrier* NewBarrier = malloc(sizeof(obj_Barrier));
            if (NewBarrier == NULL) {
                if (DEBUG_MODE) {
                    printf("Error: Could not allocate memory for new barrier.\n");
                }
                front_Quit();
            }

            if (W) {
                NewBarrier->Type = OBJ_BARRIER_TYPE_WALL;
            }
            else if (P) {
                NewBarrier->Type = OBJ_BARRIER_TYPE_PLATFORM;
            }
            else if (V) {
                NewBarrier->Type = OBJ_BARRIER_TYPE_VOID;
            }

            NewBarrier->Rect = (SDL_Rect) {
                .x = GlobalMouseX,
                .y = GlobalMouseY,
                .w = EDITOR_DEFAULT_WBARRIER_WIDTH,
                .h = EDITOR_DEFAULT_WBARRIER_HEIGHT
            };

            // Append new barrier to list
            NewBarrier->next = NULL;
            obj_Barrier* Temp = BarriersHead;
            while (Temp->next != NULL) {
                Temp = Temp->next;
            }
            Temp->next = NewBarrier;
        }
    }

    // Move camera with arrow keys
    int NewCameraX = *render_CameraX;
    int NewCameraY = *render_CameraY;
    if (InputKeys[SDL_SCANCODE_UP]) {
        NewCameraY -= EDITOR_CAMERA_SPEED;
        printf("Up arrow pressed. Moving camera position to (%d, %d).\n", NewCameraX, NewCameraY);
    }
    if (InputKeys[SDL_SCANCODE_DOWN]) {
        NewCameraY += EDITOR_CAMERA_SPEED;
        printf("Down arrow pressed. Moving camera position to (%d, %d).\n", NewCameraX, NewCameraY);
    }
    if (InputKeys[SDL_SCANCODE_LEFT]) {
        NewCameraX -= EDITOR_CAMERA_SPEED;
        printf("Left arrow pressed. Moving camera position to (%d, %d).\n", NewCameraX, NewCameraY);
    }
    if (InputKeys[SDL_SCANCODE_RIGHT]) {
        NewCameraX += EDITOR_CAMERA_SPEED;
        printf("Right arrow pressed. Moving camera position to (%d, %d).\n", NewCameraX, NewCameraY);
    }
    *render_CameraX = NewCameraX;
    *render_CameraY = NewCameraY;

    // Mouse Inputs
    bool LeftMouseButton = MouseState->ButtonStates[SDL_BUTTON_LEFT];
    if (LeftMouseButton) {
        bool FoundObj = false;
        if (Selection->SelectedBarrier == NULL && !FoundObj) {
            for (obj_Barrier* BarrierPtr = BarriersHead; BarrierPtr != NULL; BarrierPtr = BarrierPtr->next) {
                int TopLeft[2] = {BarrierPtr->Rect.x, BarrierPtr->Rect.y};
                int TopRight[2] = {BarrierPtr->Rect.x + BarrierPtr->Rect.w, BarrierPtr->Rect.y};
                int BottomLeft[2] = {BarrierPtr->Rect.x, BarrierPtr->Rect.y + BarrierPtr->Rect.h};
                int BottomRight[2] = {BarrierPtr->Rect.x + BarrierPtr->Rect.w, BarrierPtr->Rect.y + BarrierPtr->Rect.h};

                if (calc_Distance(TopLeft[0], TopLeft[1], MouseState->x, MouseState->y) < EDITOR_OBJ_DISTANCE_SNAP) {
                    Selection->SelectedBarrier = BarrierPtr;
                    Selection->SelectedPointID = EDITOR_SELECTION_ID_TOPLEFT;
                    FoundObj = true;
                    break;
                } else if (calc_Distance(TopRight[0], TopRight[1], MouseState->x, MouseState->y) < EDITOR_OBJ_DISTANCE_SNAP) {
                    Selection->SelectedBarrier = BarrierPtr;
                    Selection->SelectedPointID = EDITOR_SELECTION_ID_TOPRIGHT;
                    FoundObj = true;
                    break;
                } else if (calc_Distance(BottomLeft[0], BottomLeft[1], MouseState->x, MouseState->y) < EDITOR_OBJ_DISTANCE_SNAP) {
                    Selection->SelectedBarrier = BarrierPtr;
                    Selection->SelectedPointID = EDITOR_SELECTION_ID_BOTTOMLEFT;
                    FoundObj = true;
                    break;
                } else if (calc_Distance(BottomRight[0], BottomRight[1], MouseState->x, MouseState->y) < EDITOR_OBJ_DISTANCE_SNAP) {
                    Selection->SelectedBarrier = BarrierPtr;
                    Selection->SelectedPointID = EDITOR_SELECTION_ID_BOTTOMRIGHT;
                    FoundObj = true;
                    break;
                } else if (calc_IsPointInsideRect(MouseState->x, MouseState->y, BarrierPtr->Rect)) {
                    Selection->SelectedBarrier = BarrierPtr;
                    Selection->SelectedPointID = EDITOR_SELECTION_ID_CENTER;
                    FoundObj = true;
                    break;
                }
            }
        }
        
        if (Selection->SelectedEntity == NULL && !FoundObj) {
            for (obj_Entity* EntityPtr = EntitiesHead; EntityPtr != NULL; EntityPtr = EntityPtr->next) {
                if (calc_IsPointInsideRect(MouseState->x, MouseState->y, EntityPtr->Hitbox)) {
                    Selection->SelectedEntity = EntityPtr;
                    FoundObj = true;
                    break;
                }
            }
        }
    } else {
        if (Selection->SelectedBarrier != NULL) {
            // Normalize the barrier's rect to avoid inside-out rectangles
            SDL_Rect* Rect = &Selection->SelectedBarrier->Rect;
            if (Rect->w < 0) {
                Rect->w = -Rect->w;
                Rect->x -= Rect->w;
            }
            if (Rect->h < 0) {
                Rect->h = -Rect->h;
                Rect->y -= Rect->h;
            }
        }
        Selection->SelectedBarrier = NULL;
        Selection->SelectedPointID = EDITOR_SELECTION_ID_NONE;
        Selection->SelectedEntity = NULL;
    }


    if (Selection->SelectedBarrier != NULL) {
        // Pointers to make code more readable.
        int* RectX = &Selection->SelectedBarrier->Rect.x;
        int* RectY = &Selection->SelectedBarrier->Rect.y;
        int* RectW = &Selection->SelectedBarrier->Rect.w;
        int* RectH = &Selection->SelectedBarrier->Rect.h;
        if (Selection->SelectedPointID == EDITOR_SELECTION_ID_CENTER) {
            // Move barrier relative to the mouse position
            *RectX = MouseState->x - *RectW / 2;
            *RectY = MouseState->y - *RectH / 2;
        } else {
            // Expand barrier to the mouse position
            if (Selection->SelectedPointID == EDITOR_SELECTION_ID_TOPLEFT) {
                *RectW = *RectW - (MouseState->x - *RectX);
                *RectH = *RectH - (MouseState->y - *RectY);
                *RectX = MouseState->x;
                *RectY = MouseState->y;
            } else if (Selection->SelectedPointID == EDITOR_SELECTION_ID_TOPRIGHT) {
                *RectW = MouseState->x - *RectX;
                *RectH = *RectH + *RectY - MouseState->y;
                *RectY = MouseState->y;
            } else if (Selection->SelectedPointID == EDITOR_SELECTION_ID_BOTTOMLEFT) {
                *RectW = *RectW - (MouseState->x - *RectX);
                *RectH = MouseState->y - *RectY;
                *RectX = MouseState->x;
            } else if (Selection->SelectedPointID == EDITOR_SELECTION_ID_BOTTOMRIGHT) {
                *RectW = MouseState->x - *RectX;
                *RectH = MouseState->y - *RectY;
            }
        }
    }
    if (Selection->SelectedEntity != NULL) {
        int* HitboxX = &Selection->SelectedEntity->Hitbox.x;
        int* HitboxY = &Selection->SelectedEntity->Hitbox.y;
        int* HitboxW = &Selection->SelectedEntity->Hitbox.w;
        int* HitboxH = &Selection->SelectedEntity->Hitbox.h;
        // Move entity relative to the mouse position
        *HitboxX = MouseState->x - *HitboxW / 2;
        *HitboxY = MouseState->y - *HitboxH / 2;
    }
}