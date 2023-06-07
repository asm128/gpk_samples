#include "gpk_voxel_geometry.h"
#include "gpk_vox.h"
#include "gpk_framework.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
	::gpk::SFramework													Framework									;

	::gpk::array_obj<::gpk::SVoxelGeometry>								VOXModels									= {};
	::gpk::array_obj<::gpk::SVoxelMap<uint8_t>>							VOXModelMaps								= {};
	::gpk::array_obj<::gpk::vcc>										VOXModelNames								= {};

	int32_t																IdViewport									= -1;
	::gpk::pobj<::gpk::rtbgra8d32>	BackBuffer;

	HDC																	DrawingContext								= {};
	HGLRC																GLRenderContext								= {}; 
 
																		SApplication								(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

static const ::gpk::vcs								fileNames []								= 
	{ "alien_bot1.vox"
	, "alien_bot1.vox"
	, "alien_bot2.vox"
	//, "alien_bot3.vox"
	//, "alien_crawl1.vox"
	////, "alien_crawl2.vox"
	//, "alien_egg1.vox"
	////, "alien_egg2.vox"
	////, "alien_egg3.vox"
	//, "alien_engi1a.vox"
	////, "alien_engi1b.vox"
	////, "alien_engi1c.vox"
	////, "alien_engi2a.vox"
	//, "alien_engi2b.vox"
	////, "alien_engi2c.vox"
	////, "alien_engi2d.vox"
	//, "alien_engi3.vox"
	//, "alien_eye1a.vox"
	////, "alien_eye1b.vox"
	////, "alien_eye1c.vox"
	//, "alien_infected1.vox"
	////, "alien_infected2.vox"
	////, "alien_infected3.vox"
	//, "alien_lift.vox"
	//, "alien_saucer1a.vox"
	////, "alien_saucer1b.vox"
	////, "alien_saucer1c.vox"
	////, "alien_saucer2a.vox"
	////, "alien_saucer2b.vox"
	////, "alien_saucer2c.vox"
	////, "alien_saucer3a.vox"
	////, "alien_saucer3b.vox"
	////, "alien_saucer3c.vox"
	////, "alien_tool1.vox"
	////, "alien_tool2.vox"
	////, "alien_tool3.vox"
	//, "chr_army1.vox"
	////, "chr_army1a.vox"
	////, "chr_army1b.vox"
	////, "chr_army2.vox"
	//, "chr_army2a.vox"
	////, "chr_army2b.vox"
	//, "chr_army3.vox"
	////, "chr_army4.vox"
	////, "chr_army4a.vox"
	//, "chr_base.vox"
	//, "chr_beardo1.vox"
	////, "chr_beardo2.vox"
	////, "chr_beardo3.vox"
	////, "chr_beardo4.vox"
	//, "chr_beau.vox"
	//, "chr_bedroll1.vox"
	////, "chr_bedroll2.vox"
	//, "chr_bridget.vox"
	//, "chr_bro.vox"
	//, "chr_brookie.vox"
	//, "chr_butcher.vox"
	//, "chr_chef.vox"
	//, "chr_cop1.vox"
	////, "chr_cop2.vox"
	//, "chr_costume1.vox"
	////, "chr_costume2.vox"
	////, "chr_costume3.vox"
	////, "chr_costume4.vox"
	//, "chr_eskimo.vox"
	//, "chr_fatkid.vox"
	//, "chr_goth1.vox"
	////, "chr_goth2.vox"
	////, "chr_goth3.vox"
	////, "chr_hazmat1.vox"
	//, "chr_hazmat2.vox"
	//, "chr_headphones.vox"
	//, "chr_hobo1.vox"
	//, "chr_hunter1.vox"
	////, "chr_hunter2.vox"
	//, "chr_janitor.vox"
	//, "chr_lady1.vox"
	////, "chr_lady2.vox"
	////, "chr_lady3.vox"
	////, "chr_lady4.vox"
	//, "chr_mailman.vox"
	//, "chr_mayor.vox"
	//, "chr_mechanic.vox"
	//, "chr_mike.vox"
	//, "chr_mission1.vox"
	////, "chr_mission2.vox"
	//, "chr_naked1.vox"
	////, "chr_naked2.vox"
	////, "chr_naked3.vox"
	////, "chr_naked4.vox"
	////, "chr_naked5.vox"
	////, "chr_naked6.vox"
	//, "chr_nun.vox"
	//, "chr_nurse.vox"
	//, "chr_paramedic1.vox"
	////, "chr_paramedic2.vox"
	//, "chr_ponytail1.vox"
	////, "chr_ponytail2.vox"
	////, "chr_ponytail3.vox"
	//, "chr_priest.vox"
	//, "chr_punk.vox"
	//, "chr_raver1.vox"
	////, "chr_raver2.vox"
	////, "chr_raver3.vox"
	//, "chr_riotcop.vox"
	//, "chr_robot.vox"
	//, "chr_scientist.vox"
	//, "chr_sign1.vox"
	////, "chr_sign2.vox"
	//, "chr_sports1.vox"
	////, "chr_sports2.vox"
	////, "chr_sports3.vox"
	////, "chr_sports4.vox"
	//, "chr_suit1.vox"
	////, "chr_suit2.vox"
	////, "chr_suit3.vox"
	////, "chr_suit4.vox"
	//, "chr_sumo1.vox"
	////, "chr_sumo2.vox"
	////, "chr_super1.vox"
	////, "chr_super2.vox"
	////, "chr_super3.vox"
	////, "chr_super4.vox"
	////, "chr_super5.vox"
	//, "chr_thief.vox"
	//, "chr_worker1.vox"
	////, "chr_worker2.vox"
	////, "chr_worker3.vox"
	//, "chr_zombie1.vox"
	////, "chr_zombie2.vox"
	////, "chr_zombie3.vox"
	////, "chr_zombie4.vox"
	//, "env_crete1a.vox"
	////, "env_crete1b.vox"
	////, "env_crete1c.vox"
	////, "env_crete1d.vox"
	////, "env_crete2a.vox"
	////, "env_crete2b.vox"
	////, "env_crete2c.vox"
	////, "env_crete2d.vox"
	//, "env_grass1a.vox"
	////, "env_grass1b.vox"
	////, "env_grass1c.vox"
	////, "env_grass1d.vox"
	////, "mob_bear.vox"
	//, "mob_cat1.vox"
	////, "mob_cat2.vox"
	////, "mob_cat3.vox"
	////, "mob_cat4.vox"
	//, "mob_dog1.vox"
	////, "mob_dog2.vox"
	//, "mob_penguin.vox"
	//, "obj_arcade1.vox"
	////, "obj_arcade2.vox"
	////, "obj_arcade3.vox"
	////, "obj_arcade4.vox"
	////, "obj_arcade5.vox"
	//, "obj_armgate1.vox"
	////, "obj_armgate2.vox"
	//, "obj_bench1.vox"
	////, "obj_bench2.vox"
	////, "obj_bench3.vox"
	////, "obj_bench4.vox"
	////, "obj_bench5.vox"
	//, "obj_boxingring.vox"
	//, "obj_busstop.vox"
	//, "obj_campfire.vox"
	//, "obj_candle.vox"
	//, "obj_cart1.vox"
	////, "obj_cart1a.vox"
	////, "obj_cart1b.vox"
	////, "obj_cart2.vox"
	////, "obj_cart2a.vox"
	////, "obj_cart2b.vox"
	//, "obj_celltower.vox"
	//, "obj_chair1.vox"
	////, "obj_chair2.vox"
	//, "obj_christmas1.vox"
	//, "obj_column1.vox"
	////, "obj_column2.vox"
	////, "obj_column3.vox"
	//, "obj_cone1.vox"
	//, "obj_container1.vox"
	////, "obj_container2.vox"
	////, "obj_container3.vox"
	////, "obj_container4.vox"
	//, "obj_cross.vox"
	//, "obj_crosswalk.vox"
	//, "obj_curb1.vox"
	////, "obj_curb2.vox"
	////, "obj_curb3.vox"
	////, "obj_curb4.vox"
	////, "obj_curb5.vox"
	////, "obj_curb6.vox"
	////, "obj_curb7.vox"
	////, "obj_curb7a.vox"
	////, "obj_curb8.vox"
	//, "obj_dogstand.vox"
	//, "obj_door1.vox"
	////, "obj_door2.vox"
	////, "obj_door3.vox"
	////, "obj_door4.vox"
	//, "obj_driveway1.vox"
	////, "obj_driveway2.vox"
	////, "obj_driveway3.vox"
	//, "obj_fence1.vox"
	////, "obj_fence2.vox"
	////, "obj_fence3.vox"
	////, "obj_fence4.vox"
	////, "obj_fence5.vox"
	////, "obj_fence6.vox"
	////, "obj_fence7.vox"
	//, "obj_fire1.vox"
	////, "obj_fire2.vox"
	////, "obj_fire3.vox"
	////, "obj_fire4.vox"
	////, "obj_fire5.vox"
	//, "obj_fountain.vox"
	//, "obj_grave1.vox"
	////, "obj_grave2.vox"
	////, "obj_grave3.vox"
	////, "obj_grave4.vox"
	//, "obj_grill.vox"
	//, "obj_guitarcase.vox"
	//, "obj_halo.vox"
	//, "obj_house1.vox"
	////, "obj_house1a.vox"
	////, "obj_house1b.vox"
	////, "obj_house1c.vox"
	////, "obj_house2.vox"
	//, "obj_house2a.vox"
	////, "obj_house2b.vox"
	////, "obj_house2c.vox"
	////, "obj_house2d.vox"
	////, "obj_house3.vox"
	////, "obj_house3a.vox"
	//, "obj_house3b.vox"
	////, "obj_house3c.vox"
	////, "obj_house4.vox"
	////, "obj_house4a.vox"
	////, "obj_house4b.vox"
	//, "obj_house4c.vox"
	////, "obj_house4d.vox"
	////, "obj_house5.vox"
	////, "obj_house5a.vox"
	////, "obj_house5b.vox"
	////, "obj_house5c.vox"
	////, "obj_house6.vox"
	////, "obj_house6a.vox"
	////, "obj_house6b.vox"
	////, "obj_house6c.vox"
	//, "obj_house6d.vox"
	////, "obj_house7.vox"
	////, "obj_house7a.vox"
	////, "obj_house7b.vox"
	////, "obj_house7c.vox"
	////, "obj_house8.vox"
	////, "obj_house8a.vox"
	////, "obj_house8b.vox"
	////, "obj_house8c.vox"
	//, "obj_hydrant.vox"
	//, "obj_mailbox.vox"
	////, "obj_mailbox2.vox"
	////, "obj_mailbox2a.vox"
	////, "obj_mailbox2b.vox"
	//, "obj_mushroom1.vox"
	////, "obj_mushroom2.vox"
	////, "obj_mushroom3.vox"
	//, "obj_newsbox1.vox"
	////, "obj_newsbox2.vox"
	////, "obj_newsbox3.vox"
	////, "obj_newsbox4.vox"
	//, "obj_park_block.vox"
	//, "obj_path1.vox"
	//, "obj_pentagram.vox"
	//, "obj_planter1.vox"
	////, "obj_planter2.vox"
	////, "obj_planter3a.vox"
	////, "obj_planter3b.vox"
	//, "obj_playgrnd1.vox"
	////, "obj_playgrnd2.vox"
	////, "obj_playgrnd3.vox"
	////, "obj_playgrnd4.vox"
	////, "obj_playgrnd5.vox"
	//, "obj_policetape.vox"
	//, "obj_potty1.vox"
	////, "obj_potty2.vox"
	////, "obj_potty3.vox"
	//, "obj_pumpkin.vox"
	//, "obj_rubbish1.vox"
	////, "obj_rubbish2.vox"
	////, "obj_rubbish3.vox"
	////, "obj_rubbish4.vox"
	//, "obj_sidewalk1.vox"
	////, "obj_sidewalk2.vox"
	////, "obj_sidewalk3.vox"
	////, "obj_sidewalk4.vox"
	////, "obj_sidewalk5.vox"
	//, "obj_sign1.vox"
	////, "obj_sign2.vox"
	////, "obj_sign3.vox"
	////, "obj_sign4.vox"
	////, "obj_sign5.vox"
	////, "obj_sign6.vox"
	////, "obj_sign7.vox"
	////, "obj_sign8.vox"
	////, "obj_sign9.vox"
	//, "obj_splatter1.vox"
	////, "obj_splatter2.vox"
	////, "obj_splatter3.vox"
	//, "obj_stage.vox"
	//, "obj_statue1.vox"
	////, "obj_statue2.vox"
	////, "obj_statue3.vox"
	//, "obj_stlight1.vox"
	////, "obj_stlight2.vox"
	////, "obj_stlight3.vox"
	//, "obj_store01.vox"
	////, "obj_store02.vox"
	////, "obj_store03.vox"
	//, "obj_store03a.vox"
	////, "obj_store04.vox"
	////, "obj_store05.vox"
	////, "obj_store06.vox"
	////, "obj_store07.vox"
	////, "obj_store08.vox"
	////, "obj_store09.vox"
	////, "obj_store10.vox"
	////, "obj_store11.vox"
	////, "obj_store12.vox"
	////, "obj_store13.vox"
	////, "obj_store14.vox"
	////, "obj_store15.vox"
	////, "obj_store16.vox"
	////, "obj_store16a.vox"
	//, "obj_store16b.vox"
	////, "obj_store17.vox"
	////, "obj_store17a.vox"
	//, "obj_story01.vox"
	////, "obj_story01a.vox"
	////, "obj_story01b.vox"
	////, "obj_story02.vox"
	////, "obj_story03.vox"
	//, "obj_story03a.vox"
	////, "obj_story03b.vox"
	////, "obj_story03c.vox"
	////, "obj_story03d.vox"
	////, "obj_story04.vox"
	////, "obj_story04a.vox"
	//, "obj_story04b.vox"
	////, "obj_story04c.vox"
	////, "obj_story04d.vox"
	////, "obj_story05.vox"
	////, "obj_story05a.vox"
	////, "obj_story06.vox"
	////, "obj_story06a.vox"
	////, "obj_story06b.vox"
	//, "obj_story06c.vox"
	////, "obj_story06d.vox"
	//, "obj_street1.vox"
	////, "obj_street2.vox"
	//, "obj_stretcher.vox"
	//, "obj_table1.vox"
	////, "obj_table2.vox"
	////, "obj_table3.vox"
	////, "obj_table3a.vox"
	////, "obj_table3b.vox"
	//, "obj_tracks1.vox"
	////, "obj_tracks2.vox"
	//, "obj_trashcan1.vox"
	////, "obj_trashcan2.vox"
	////, "obj_trashcan3.vox"
	////, "obj_trashcan4.vox"
	//, "obj_tree1.vox"
	////, "obj_tree1a.vox"
	////, "obj_tree1b.vox"
	////, "obj_tree1c.vox"
	////, "obj_tree2.vox"
	////, "obj_tree2a.vox"
	////, "obj_tree2b.vox"
	////, "obj_tree2c.vox"
	////, "obj_tree3.vox"
	////, "obj_tree4.vox"
	//, "obj_trellis.vox"
	//, "obj_trlight1.vox"
	////, "obj_trlight2.vox"
	//, "obj_wall.vox"
	//, "piano.vox"
	////, "scene_aliens.vox"
	////, "scene_arcade.vox"
	////, "scene_army.vox"
	////, "scene_bus.vox"
	////, "scene_car.vox"
	////, "scene_carflip.vox"
	////, "scene_checkpoint.vox"
	////, "scene_christmas.vox"
	////, "scene_church1.vox"
	////, "scene_coffee.vox"
	////, "scene_corner.vox"
	////, "scene_crucifix.vox"
	////, "scene_depot.vox"
	////, "scene_depot2.vox"
	////, "scene_depot3.vox"
	////, "scene_fall.vox"
	////, "scene_grave.vox"
	////, "scene_hazmat.vox"
	////, "scene_hazmat2.vox"
	////, "scene_headache.vox"
	////, "scene_house.vox"
	////, "scene_house2.vox"
	////, "scene_house3.vox"
	////, "scene_house4.vox"
	////, "scene_house5.vox"
	////, "scene_house6.vox"
	////, "scene_house7.vox"
	////, "scene_hunt.vox"
	////, "scene_lunch.vox"
	////, "scene_mechanic.vox"
	////, "scene_mechanic2.vox"
	////, "scene_mobile.vox"
	////, "scene_orgy.vox"
	////, "scene_parade.vox"
	////, "scene_park.vox"
	////, "scene_park2.vox"
	////, "scene_park3.vox"
	////, "scene_park4.vox"
	////, "scene_parked.vox"
	////, "scene_protest.vox"
	////, "scene_riot.vox"
	////, "scene_sacrifice.vox"
	////, "scene_ships.vox"
	////, "scene_sidewalk.vox"
	////, "scene_store.vox"
	////, "scene_store10.vox"
	////, "scene_store11.vox"
	////, "scene_store2.vox"
	////, "scene_store3.vox"
	////, "scene_store4.vox"
	////, "scene_store5.vox"
	////, "scene_store6.vox"
	////, "scene_store7.vox"
	////, "scene_store8.vox"
	////, "scene_store9.vox"
	////, "scene_sumo.vox"
	////, "scene_tentcity.vox"
	////, "scene_tentcity2.vox"
	////, "scene_theater.vox"
	////, "scene_train.vox"
	////, "scene_vehicles1.vox"
	////, "scene_vehicles2.vox"
	////, "scene_zombies.vox"
	//, "veh_ambulance.vox"
	//, "veh_bus.vox"
	//, "veh_cab1.vox"
	//, "veh_car1.vox"
	//////, "veh_car2.vox"
	//////, "veh_car3.vox"
	//////, "veh_car4.vox"
	//////, "veh_car5.vox"
	//, "veh_fire.vox"
	//, "veh_lunch1.vox"
	//////, "veh_lunch2.vox"
	//////, "veh_lunch3.vox"
	//////, "veh_lunch4.vox"
	//, "veh_mini1.vox"
	//////, "veh_mini2.vox"
	//////, "veh_mini3.vox"
	//////, "veh_mini4.vox"
	//////, "veh_mini5.vox"
	//, "veh_police1.vox"
	//, "veh_suv1.vox"
	////, "veh_suv2.vox"
	////, "veh_suv3.vox"
	//, "veh_tank1.vox"
	//, "veh_train.vox"
	////, "veh_train2.vox"
	////, "veh_train3.vox"
	//, "veh_truck1.vox"
	////, "veh_truck2.vox"
	////, "veh_truck3.vox"
	////, "veh_truck4.vox"
	////, "veh_truck5.vox"
	////, "veh_truck6.vox"
	////, "veh_truck7.vox"
	//, "veh_wagon1.vox"
	////, "veh_wagon2.vox"
	////, "veh_wagon3.vox"
	////, "veh_wagon4.vox"
	};

#endif // APPLICATION_H_098273498237423
