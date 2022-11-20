#ifndef _GSC_ENTITY_HPP_
#define _GSC_ENTITY_HPP_

/* gsc functions */
#include "gsc.hpp"

void gsc_entity_setalive(scr_entref_t id);
void gsc_entity_setbounds(scr_entref_t id);
void gsc_entity_gettagangles(scr_entref_t id);
void gsc_entity_gettagorigin(scr_entref_t id);
void gsc_entity_hastag(scr_entref_t id);
void gsc_entity_islinkedto(scr_entref_t id);
void gsc_entity_getturretowner(scr_entref_t id);
void gsc_entity_addgrenadefusetime(scr_entref_t id);

#endif