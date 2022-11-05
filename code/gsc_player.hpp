#ifndef _GSC_PLAYER_HPP_
#define _GSC_PLAYER_HPP_

#include "gsc.hpp"

void gsc_player_velocity_set(scr_entref_t id);
void gsc_player_velocity_add(scr_entref_t id);
void gsc_player_velocity_get(scr_entref_t id);
void gsc_player_button_ads(scr_entref_t id);
void gsc_player_button_left(scr_entref_t id);
void gsc_player_button_right(scr_entref_t id);
void gsc_player_button_forward(scr_entref_t id);
void gsc_player_button_back(scr_entref_t id);
void gsc_player_button_leanleft(scr_entref_t id);
void gsc_player_button_leanright(scr_entref_t id);
void gsc_player_button_jump(scr_entref_t id);
void gsc_player_button_reload(scr_entref_t id);
void gsc_player_button_frag(scr_entref_t id);
void gsc_player_button_smoke(scr_entref_t id);
void gsc_player_stance_get(scr_entref_t id);
void gsc_player_stance_set(scr_entref_t id);
void gsc_player_spectatorclient_get(scr_entref_t id);
void gsc_player_get_userinfo(scr_entref_t id);
void gsc_player_set_userinfo(scr_entref_t id);
void gsc_player_getip(scr_entref_t id);
void gsc_player_getping(scr_entref_t id);
void gsc_player_clientuserinfochanged(scr_entref_t id);
void gsc_player_clientcommand(scr_entref_t id);
void gsc_player_getlastconnecttime(scr_entref_t id);
void gsc_player_getlastmsg(scr_entref_t id);
void gsc_player_getclientconnectstate(scr_entref_t id);
void gsc_player_addresstype(scr_entref_t id);
void gsc_player_renameclient(scr_entref_t id);
void gsc_player_outofbandprint(scr_entref_t id);
void gsc_player_connectionlesspacket(scr_entref_t id);
void gsc_player_resetnextreliabletime(scr_entref_t id);
void gsc_player_ismantling(scr_entref_t id);
void gsc_player_isonladder(scr_entref_t id);
void gsc_player_isusingturret(scr_entref_t id);
void gsc_player_getjumpslowdowntimer(scr_entref_t id);
void gsc_player_clearjumpstate(scr_entref_t id);
void gsc_player_setjump_slowdownenable(scr_entref_t id);
void gsc_player_setjump_height(scr_entref_t id);
void gsc_player_setg_speed(scr_entref_t id);
void gsc_player_setg_gravity(scr_entref_t id);
void gsc_player_getg_speed(scr_entref_t id);
void gsc_player_getg_gravity(scr_entref_t id);
void gsc_player_setfirethroughwalls(scr_entref_t id);
void gsc_player_setfirerangescale(scr_entref_t id);
void gsc_player_setweaponfiremeleedelay(scr_entref_t id);
void gsc_player_setmeleeheightscale(scr_entref_t id);
void gsc_player_setmeleerangescale(scr_entref_t id);
void gsc_player_setmeleewidthscale(scr_entref_t id);
void gsc_player_getweaponindexoffhand(scr_entref_t id);
void gsc_player_getcurrentoffhandslotammo(scr_entref_t id);
void gsc_player_set_anim(scr_entref_t id);
void gsc_player_getcooktime(scr_entref_t id);
void gsc_player_setguid(scr_entref_t id);
void gsc_player_clienthasclientmuted(scr_entref_t id);
void gsc_player_muteclient(scr_entref_t id);
void gsc_player_unmuteclient(scr_entref_t id);
void gsc_player_getlastgamestatesize(scr_entref_t id);
void gsc_player_getfps(scr_entref_t id);
void gsc_player_lookatkiller(scr_entref_t id);
void gsc_player_isbot(scr_entref_t id);
void gsc_player_item_pickup(scr_entref_t id);
void gsc_player_kick2();
void gsc_player_noclip(scr_entref_t id);
void gsc_player_getinactivitytime(scr_entref_t id);
void gsc_player_set_earthquakes(scr_entref_t id);
void gsc_player_earthquakeforplayer(scr_entref_t id);
void gsc_player_playfxforplayer(scr_entref_t id);
void gsc_player_playfxontagforplayer(scr_entref_t id);
void gsc_player_getclienthudelemcount(scr_entref_t id);
void gsc_player_runscriptanimation(scr_entref_t id);
void gsc_player_silent(scr_entref_t id);
void gsc_player_getgroundentity(scr_entref_t id);
void gsc_player_getentertime(scr_entref_t id);
void gsc_player_stopuseturret(scr_entref_t id);
void gsc_player_getplayerstateflags(scr_entref_t id);
void gsc_player_objective_player_add(scr_entref_t id);
void gsc_player_objective_player_delete(scr_entref_t id);
void gsc_player_objective_player_icon(scr_entref_t id);
void gsc_player_objective_player_position(scr_entref_t id);
void gsc_player_objective_player_state(scr_entref_t id);
void gsc_player_getvieworigin(scr_entref_t id);
#if COMPILE_CUSTOM_VOICE == 1
void gsc_player_playsoundfile(scr_entref_t id);
void gsc_player_stopsoundfile(scr_entref_t id);
void gsc_player_getremainingsoundfileduration(scr_entref_t id);
void gsc_player_isplayingsoundfile(scr_entref_t id);
#endif

#endif