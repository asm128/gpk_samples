#include "ced_demo_13_game.h"
#include <windows.h>
#include <mmsystem.h>

int													ssg::setupStars			(::ssg::SStars & stars, ::gpk::SCoord2<uint32_t> targetSize)	{
	if(0 == targetSize.y) return 0;
	if(0 == targetSize.x) return 0;
	stars.Speed		.resize(128);
	stars.Brightness.resize(128);
	stars.Position	.resize(128);
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		stars.Speed			[iStar]							= float(16 + (rand() % 64));
		stars.Brightness	[iStar]							= float(1.0 / RAND_MAX * rand());
		stars.Position		[iStar].y						= float(rand() % targetSize.y);
		stars.Position		[iStar].x						= float(rand() % targetSize.x);
	}
	return 0;
}

static	int											shipCreate			(::ssg::SSolarSystem & solarSystem, int32_t teamId, int32_t iGeometry, int32_t iImage)	{
	::ssg::SShipScene										& scene				= solarSystem.Scene;
	const uint32_t											countParts			= 6;

	::ssg::SShip											ship				= {};
	{	// Create main ship entity
		::ssg::SEntity											entity				= {-1};
		entity												= {-1};
		entity.Geometry										= -1;	//1 + (iGeometry % 5);
		entity.Transform									= scene.ModelMatricesGlobal.push_back(solarSystem.ShipPhysics.MatrixIdentity4);
		entity.Image										= -1;	//iImage % 5;
		entity.Body											= solarSystem.ShipPhysics.Spawn();
		ship.Entity											= solarSystem.Entities.push_back(entity);
		ship.Team											= teamId;
		const int32_t											indexBody			= solarSystem.ShipPhysics.Spawn();
	}
	const int32_t											indexShip			= solarSystem.Ships.push_back(ship);
	//ship.Parts.reserve(countParts);
	for(uint32_t iPart = 0; iPart < countParts; ++iPart) {	// Create child parts
		::ssg::SEntity											entityOrbit				= {ship.Entity};
		::ssg::SEntity											entityPart				= {-1};
		entityOrbit.Parent									= ship.Entity;
		entityOrbit.Geometry								= -1;
		entityOrbit.Transform								= scene.ModelMatricesGlobal.push_back(solarSystem.ShipPhysics.MatrixIdentity4);
		entityOrbit.Image									= -1;
		entityOrbit.Body									= ::gpk::createOrbiter(solarSystem.ShipPhysics
			, 1		//PLANET_MASSES				[iPlanet]
			, 2.5	//PLANET_DISTANCE			[iPlanet]
			, 0		//PLANET_AXIALTILT			[iPlanet]
			, 1		//PLANET_DAY				[iPlanet]
			, 1		//PLANET_DAY				[PLANET_EARTH]
			, 6		//PLANET_ORBITALPERIOD		[iPlanet]
			, 0		//PLANET_ORBITALINCLINATION	[iPlanet]
			, 1		//1.0 / PLANET_DISTANCE		[PLANET_COUNT - 1] * 2500
			);
		entityPart.Parent									= solarSystem.Entities.push_back(entityOrbit);
		entityPart.Geometry									= 1 + ((iGeometry + (iPart % 2)) % 5);
		entityPart.Transform								= scene.ModelMatricesGlobal.push_back(solarSystem.ShipPhysics.MatrixIdentity4);
		entityPart.Image									= (4 + iImage - (iPart % 2)) % 32;
		entityPart.Body										= entityOrbit.Body + 1;
		int32_t													indexEntityPart				= solarSystem.Entities.push_back(entityPart);
		solarSystem.Entities[entityPart.Parent].Children.push_back(indexEntityPart);
		solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation	= {};
		solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.MakeFromEulerTaitBryan(0, (float)(::gpk::math_2pi / countParts * iPart), 0);
		//solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.Normalize();

		::ssg::SShipPart										shipPart				= {};
		shipPart.Entity										= entityPart.Parent;
		::ssg::SEntity											& parentEntity			= solarSystem.Entities[ship.Entity];
 		parentEntity.Children.push_back(shipPart.Entity);

		::ssg::SShip											& parentShip			= solarSystem.Ships[indexShip];
		parentShip.Parts.push_back(shipPart);
	}
	return indexShip;
}

//static	int											engineCreate		(::ssg::SSolarSystem & solarSystem, int32_t iImage)	{
//	return 0;
//}

static	int											shipGeometryBuildEngine	(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildHelix(geometry, 8U, 8U, 0.15f, {0, 1.1f, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 6U, .05f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, {0, 1, 0}, {-1, 1, 1});
	return 0;
}

static	int											shipGeometryBuildWafer	(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildCube(geometry, {1.2f, 0.5f, 1.2f});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, { 0.75f, .75f}, { 1.5, 1, 1.5});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, {-0.75f, .75f}, {-1.5, 1, 1.5});
	return 0;
}
static	int											shipGeometryBuildGun	(::gpk::SGeometryQuads & geometry)						{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .7f, {0, 0});
	::gpk::geometryBuildFigure0	(geometry, 2U, 8U, 1, {});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, {0, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, {0, 0}, {-1, 1, 1});
	return 0;
}
static	int											shipGeometryBuildCannon	(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .8f, {0, 0});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, { .25, .8f, .25});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, {-.25, .8f, .25});
	return 0;
}

//static	int											shipGeometryBuildSilo	(::gpk::SGeometryQuads & geometry)			{ return 0; }
//static	int											shipGeometryBuildShield	(::gpk::SGeometryQuads & geometry)			{ return 0; }

static	int											modelsSetup				(::ssg::SShipScene & scene)			{
	scene.Geometry.resize(6);

	::shipGeometryBuildEngine	(scene.Geometry[0]);
	::shipGeometryBuildGun		(scene.Geometry[1]);
	::shipGeometryBuildWafer	(scene.Geometry[2]);
	::shipGeometryBuildCannon	(scene.Geometry[3]);

	::gpk::geometryBuildCube	(scene.Geometry[4], {1, 1, 1});
	::gpk::geometryBuildSphere	(scene.Geometry[4], 4U, 2U, 1, {0, 0});
	::gpk::geometryBuildSphere	(scene.Geometry[5], 6U, 2U, 1, {0, 0});

	{
		::gpk::SColorFloat										baseColor	[]			=
			{ ::gpk::LIGHTGREEN
			, ::gpk::LIGHTBLUE
			, ::gpk::LIGHTRED
			, ::gpk::LIGHTCYAN
			, ::gpk::LIGHTORANGE
			, ::gpk::LIGHTYELLOW
			, ::gpk::YELLOW
			, ::gpk::RED
			, ::gpk::BLUE
			, ::gpk::BROWN
			, ::gpk::GRAY
			, ::gpk::PANOCHE
			, ::gpk::TURQUOISE
			};

		scene.Image.resize(32);
		for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
			::gpk::SImage<::gpk::SColorBGRA>						& image							= scene.Image[iImage];
			if(image.Texels.size())
				continue;
			image.resize(32, 5);
			for(uint32_t y = 0; y < image.metrics().y; ++y) {// Generate noise color for planet texture
				bool													xAffect						= (y % 2);
				::gpk::SColorFloat										lineColor					= baseColor[(iImage + (rand() % 4)) % (::gpk::size(baseColor) - 4)];
				for(uint32_t x = 0; x < image.metrics().x; ++x) {
					image.Texels[y * image.metrics().x + x]				= lineColor * (xAffect ? ::gpk::max(.25, ::std::sin(x * (1.0 / image.metrics().x * ::gpk::math_pi))) : 1);
				}
			}
		}
	}
	return 0;
}

int													ssg::solarSystemSetupBackgroundImage	(::gpk::SImage<::gpk::SColorBGRA> & backgroundImage, ::gpk::SCoord2<uint32_t> windowSize)	{
	backgroundImage.resize(windowSize);
	const ::gpk::SColorBGRA									colorBackground							= {0x20, 0x8, 0x4};
	//colorBackground									+= (colorBackground * (0.5 + (0.5 / 65535 * rand())) * ((rand() % 2) ? -1 : 1)) ;
	for(uint32_t y = 0; y < windowSize.y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < windowSize.x; ++x)
		backgroundImage.Texels[y * windowSize.x + x]		= colorBackground;
	return 0;
}

int													ssg::stageSetup							(::ssg::SSolarSystem & solarSystem)	{	// Set up enemy ships
	static constexpr	const uint32_t						partHealth								= 10;

	struct SShipPartSetup {
		::ssg::SHIP_PART_TYPE	Type	;
		::ssg::WEAPON_TYPE		Weapon	;
		::ssg::MUNITION_TYPE	Munition;
		double					MaxDelay;
		int32_t					Damage	;
	};

	static constexpr const SShipPartSetup					weaponDefinitions		[]				=
		{ {::ssg::SHIP_PART_TYPE_GUN	, ::ssg::WEAPON_TYPE_GUN		, ::ssg::MUNITION_TYPE_BULLET	, .1,  1}
		, {::ssg::SHIP_PART_TYPE_GUN	, ::ssg::WEAPON_TYPE_SHOTGUN	, ::ssg::MUNITION_TYPE_BULLET	, .3,  1}
		, {::ssg::SHIP_PART_TYPE_WAFER	, ::ssg::WEAPON_TYPE_GUN		, ::ssg::MUNITION_TYPE_RAY		, .3,  3}
		, {::ssg::SHIP_PART_TYPE_WAFER	, ::ssg::WEAPON_TYPE_SHOTGUN	, ::ssg::MUNITION_TYPE_RAY		, .6,  3}
		, {::ssg::SHIP_PART_TYPE_CANNON	, ::ssg::WEAPON_TYPE_CANNON		, ::ssg::MUNITION_TYPE_SHELL	,  2, 10}
		, {::ssg::SHIP_PART_TYPE_CANNON	, ::ssg::WEAPON_TYPE_CANNON		, ::ssg::MUNITION_TYPE_WAVE		,  2,  1}
		, {::ssg::SHIP_PART_TYPE_CANNON	, ::ssg::WEAPON_TYPE_CANNON		, ::ssg::MUNITION_TYPE_FLARE	,  2,  1}
		, {::ssg::SHIP_PART_TYPE_SILO	, ::ssg::WEAPON_TYPE_ROCKET		, ::ssg::MUNITION_TYPE_FLARE	,  2,  3}
		, {::ssg::SHIP_PART_TYPE_SHIELD	, ::ssg::WEAPON_TYPE_SHIELD		, ::ssg::MUNITION_TYPE_FLARE	,  2,  3}
		, {::ssg::SHIP_PART_TYPE_SILO	, ::ssg::WEAPON_TYPE_CANNON		, ::ssg::MUNITION_TYPE_RAY		,  2,  3}
		};

	if(0 == solarSystem.Ships.size()) { // Create player ship
		const int32_t											indexShip						= ::shipCreate(solarSystem, 0, 0, 0);
		::ssg::SShip											& playerShip					= solarSystem.Ships[indexShip];
		::gpk::STransform3										& shipPivot						= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[playerShip.Entity].Body];
		shipPivot.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(-::gpk::math_pi_2)});
		shipPivot.Position									= {-30};
	}
	while(((int)solarSystem.Ships.size() - 2) < (int)solarSystem.Stage) {	// Create enemy ships depending on stage.
		int32_t													indexShip						= ::shipCreate(solarSystem, 1, solarSystem.Stage + solarSystem.Ships.size(), solarSystem.Stage + solarSystem.Ships.size());
		::ssg::SShip											& enemyShip						= solarSystem.Ships[indexShip];
		::gpk::STransform3										& shipTransform					= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[enemyShip.Entity].Body];
		shipTransform.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(::gpk::math_pi_2)});
		shipTransform.Position								= {5.0f + 5 * solarSystem.Ships.size()};
		for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart)
			solarSystem.ShipPhysics.Forces[solarSystem.Entities[enemyShip.Parts[iPart].Entity].Body].Rotation.y	*= float(1 + indexShip * .35);
	}

	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::ssg::SShip										& ship							= solarSystem.Ships[iShip];
		ship.Health										= 0;
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			::ssg::SShipPart									& shipPart						= ship.Parts[iPart];
			shipPart.Health									= partHealth;
			//ship.Team										= iShip ? 1 : 0;
			int32_t												weapon							= 0;
			if(0 == ship.Team)
				weapon											= (iPart % 5);
			else {
				weapon											= (0 == (iShip % 7) && 0 == iPart) ?
					(	(0 == (solarSystem.Stage % 11 )) ? (iShip / 5 - ((iShip >= 5) ? 1: 0)) % 4
					:	(0 == (solarSystem.Stage % 7  )) ? (iShip / 5 - ((iShip >= 5) ? 1: 0)) % 4
					:	(0 == (solarSystem.Stage % 5  )) ? (iShip / 5 - ((iShip >= 5) ? 1: 0)) % 4
					:	(0 == (solarSystem.Stage % 3  )) ? (iShip / 5 - ((iShip >= 5) ? 1: 0)) % 4
					:	4
					)
				:	4
				;
			}
			shipPart.Type									= weaponDefinitions[weapon].Type;
			shipPart.Shots.MaxDelay							= weaponDefinitions[weapon].MaxDelay;
			if(0 != ship.Team)
				shipPart.Shots.MaxDelay							*= 1 + (2 * iPart);
			shipPart.Shots.Weapon							= weaponDefinitions[weapon].Weapon;
			shipPart.Shots.Type								= weaponDefinitions[weapon].Munition;
			shipPart.Shots.Damage							= weaponDefinitions[weapon].Damage;
			shipPart.Shots.Delay							= shipPart.Shots.MaxDelay / ship.Parts.size() * iPart;

				 if(shipPart.Type == ::ssg::SHIP_PART_TYPE_GUN		) { solarSystem.Entities[shipPart.Entity + 1].Geometry = 1; }
			else if(shipPart.Type == ::ssg::SHIP_PART_TYPE_WAFER	) { solarSystem.Entities[shipPart.Entity + 1].Geometry = 2; }
 			else if(shipPart.Type == ::ssg::SHIP_PART_TYPE_CANNON	) { solarSystem.Entities[shipPart.Entity + 1].Geometry = 3; }
			ship.Health										+= shipPart.Health;
		}
	}

	++solarSystem.Stage;
	solarSystem.Slowing									= true;
	PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMSTART, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	return 0;
}

int													ssg::solarSystemSetup	(::ssg::SSolarSystem & solarSystem, ::gpk::SCoord2<uint32_t> windowSize)	{
	::ssg::solarSystemSetupBackgroundImage(solarSystem.BackgroundImage, windowSize);
	::ssg::setupStars(solarSystem.Stars, windowSize);
	::ssg::SShipScene										& scene				= solarSystem.Scene;
	::modelsSetup(scene);
	::ssg::stageSetup(solarSystem);

	solarSystem.CameraSwitchDelay				= 0;
	solarSystem.Scene.Camera[CAMERA_MODE_SKY].Target				= {};
	solarSystem.Scene.Camera[CAMERA_MODE_SKY].Position				= {-0.000001f, 250, 0};
	solarSystem.Scene.Camera[CAMERA_MODE_SKY].Up					= {0, 1, 0};

	solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Target		= {};
	solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Position		= {-0.000001f, 135, 0};
	solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Up			= {0, 1, 0};
	solarSystem.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Position.RotateZ(::gpk::math_pi * .25);
	return 0;
}
