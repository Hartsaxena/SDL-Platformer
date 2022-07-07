#ifndef UPDATE_H
#define UPDATE_H

#define ENTITY_STATE_IDLE 0
// add more entity states later...


bool update_RectCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox);
bool update_EntityCheckCollision(obj_Barrier* BarriersHead, SDL_Rect Hitbox);
void update_UpdateBarrier(obj_Barrier* BarrierPtr);
void update_UpdateBarriers(obj_Barrier* BarrierHead);
void update_UpdateEntity(obj_Entity* Entity, obj_Barrier* BarriersHead);
void update_UpdateEntities(obj_Entity* EntitiesHead, obj_Barrier* BarriersHead);


#endif