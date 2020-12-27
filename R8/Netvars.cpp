#include <utility>
#include "Netvars.h"
#include "dt_recv2.h"
#include "Engine.h"

NetEntity nEntity;
NetPlayer nPlayer;
NetWeapon nWeapon;
NetObject nObject;

void NetEntity::init()
{
	netvar_tree nv;

	m_vecOrigin = nv.get_offset("DT_BaseEntity", "m_vecOrigin");
	m_vecMins = nv.get_offset("DT_BaseEntity", "m_Collision", "m_vecMins");
	m_vecMaxs = nv.get_offset("DT_BaseEntity", "m_Collision", "m_vecMaxs");
	m_iTeamNum = nv.get_offset("DT_BaseEntity", "m_iTeamNum");

	m_flSimulationTime = nv.get_offset("DT_BaseEntity", "m_flSimulationTime");
}

void NetPlayer::init()
{
	netvar_tree nv;

	m_lifeState = nv.get_offset("DT_BasePlayer", "m_lifeState");
	m_iHealth = nv.get_offset("DT_BasePlayer", "m_iHealth");
	m_fFlags = nv.get_offset("DT_BasePlayer", "m_fFlags");
	m_iClass = nv.get_offset("DT_TFPlayer", "m_PlayerClass", "m_iClass");
	m_nPlayerCond = nv.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
	m_nForceTauntCam = nv.get_offset("DT_TFPlayer", "m_nForceTauntCam");
	m_iFOV = nv.get_offset("DT_BasePlayer", "m_iFOV");
	m_flFOVRate = 0xE5C;// Broken: nv.get_offset("DT_BasePlayer", "localdata", "m_flFOVRate");
	m_iDefaultFOV = nv.get_offset("DT_BasePlayer", "m_iDefaultFOV");
	m_bGlowEnabled = nv.get_offset("DT_TFPlayer", "m_bGlowEnabled");

	m_AttributeList = nv.get_offset("DT_BasePlayer", "m_AttributeList");
	m_vecViewOffset = nv.get_offset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	deadflag = nv.get_offset("DT_BasePlayer", "pl", "deadflag");

	m_hActiveWeapon = nv.get_offset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	m_hMyWeapons = nv.get_offset("DT_BaseCombatCharacter", "m_hMyWeapons");

	m_angEyeAngles = nv.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");

	m_nTickBase = nv.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
	m_flFOVTime = nv.get_offset("DT_BasePlayer", "m_flFOVTime");
}

void NetWeapon::init()
{
	netvar_tree nv;

	m_flNextPrimaryAttack = nv.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	m_flNextSecondaryAttack = nv.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextSecondaryAttack");
	m_nNextThinkTick = nv.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_nNextThinkTick");
	m_flTimeWeaponIdle = nv.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flTimeWeaponIdle");
	m_flLastFireTime = nv.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime");
	m_iClip1 = nv.get_offset("DT_BaseCombatWeapon", "LocalWeaponData", "m_iClip1");

	m_iItemDefinitionIndex = nv.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
	m_bReadyToBackstab = nv.get_offset("DT_TFWeaponKnife", "m_bReadyToBackstab");
}

void NetObject::init()
{
	netvar_tree nv;

	m_iHealth = nv.get_offset("DT_BaseObject", "m_iHealth");
	m_iMaxHealth = nv.get_offset("DT_BaseObject", "m_iMaxHealth");
	m_bHasSapper = nv.get_offset("DT_BaseObject", "m_bHasSapper");
	m_bPlasmaDisable = nv.get_offset("DT_BaseObject", "m_bPlasmaDisable");
	m_bDisabled = nv.get_offset("DT_BaseObject", "m_bDisabled");
}

/**
* netvar_tree - Constructor
*
* Call populate_nodes on every RecvTable under client->GetAllClasses()
*/
netvar_tree::netvar_tree()
{
	const auto *client_class = gBase.Client->GetAllClasses();
	while (client_class != nullptr) {
		const auto class_info = std::make_shared<node>(0);
		auto *recv_table = client_class->Table;
		populate_nodes(recv_table, &class_info->nodes);
		nodes.emplace(recv_table->GetName(), class_info);

		client_class = client_class->pNextClass;
	}
}

/**
* populate_nodes - Populate a node map with brances
* @recv_table:	Table the map corresponds to
* @map:	Map pointer
*
* Add info for every prop in the recv table to the node map. If a prop is a
* datatable itself, initiate a recursive call to create more branches.
*/
void netvar_tree::populate_nodes(RecvTable *recv_table, map_type *map)
{
	for (auto i = 0; i < recv_table->GetNumProps(); i++) {
		const auto *prop = recv_table->GetProp(i);
		const auto prop_info = std::make_shared<node>(prop->GetOffset());

		if (prop->GetType() == DPT_DataTable)
			populate_nodes(prop->GetDataTable(), &prop_info->nodes);

		map->emplace(prop->GetName(), prop_info);
	}
}