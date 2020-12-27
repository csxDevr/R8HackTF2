#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class NetEntity
{
public:
	void init();

	DWORD m_vecOrigin;
	DWORD m_vecMins;
	DWORD m_vecMaxs;
	DWORD m_iTeamNum;

	DWORD m_flSimulationTime;
};
extern NetEntity nEntity;

class NetPlayer
{
public:
	void init();

	DWORD m_lifeState;
	DWORD m_fFlags;
	DWORD m_iHealth;
	DWORD m_iClass;
	DWORD m_nPlayerCond;
	DWORD m_nForceTauntCam;
	DWORD m_bGlowEnabled;

	DWORD m_iFOV;
	DWORD m_flFOVRate;
	DWORD m_iDefaultFOV;

	DWORD m_AttributeList;
	DWORD m_vecViewOffset;
	DWORD deadflag;
	DWORD m_hActiveWeapon;
	DWORD m_hMyWeapons;
	DWORD m_angEyeAngles;

	DWORD m_nTickBase;
	DWORD m_flFOVTime;
};
extern NetPlayer nPlayer;

class NetWeapon
{
public:
	void init();

	DWORD m_flNextPrimaryAttack;
	DWORD m_flNextSecondaryAttack;
	DWORD m_nNextThinkTick;
	DWORD m_flTimeWeaponIdle;
	DWORD m_flLastFireTime;
	DWORD m_iClip1;

	DWORD m_bReadyToBackstab;
	DWORD m_iItemDefinitionIndex;
};
extern NetWeapon nWeapon;

class NetObject
{
public:
	void init();

	DWORD m_iHealth;
	DWORD m_iMaxHealth;
	DWORD m_bHasSapper;
	DWORD m_bPlasmaDisable;
	DWORD m_bDisabled;
};
extern NetObject nObject;

class RecvTable;

class netvar_tree {
	struct node;
	using map_type = std::unordered_map<std::string, std::shared_ptr<node>>;

	struct node {
		node(int offset) : offset(offset) {}
		map_type nodes;
		int offset;
	};

	map_type nodes;

public:
	netvar_tree();

private:
	void populate_nodes(class RecvTable *recv_table, map_type *map);

	/**
	* get_offset_recursive - Return the offset of the final node
	* @map:	Node map to scan
	* @acc:	Offset accumulator
	* @name:	Netvar name to search for
	*
	* Get the offset of the last netvar from map and return the sum of it and accum
	*/
	int get_offset_recursive(map_type &map, int acc, const char *name)
	{
		return acc + map[name]->offset;
	}

	/**
	* get_offset_recursive - Recursively grab an offset from the tree
	* @map:	Node map to scan
	* @acc:	Offset accumulator
	* @name:	Netvar name to search for
	* @args:	Remaining netvar names
	*
	* Perform tail recursion with the nodes of the specified branch of the tree passed for map
	* and the offset of that branch added to acc
	*/
	template<typename ...args_t>
	int get_offset_recursive(map_type &map, int acc, const char *name, args_t ...args)
	{
		const auto &node = map[name];
		return get_offset_recursive(node->nodes, acc + node->offset, args...);
	}

public:
	/**
	* get_offset - Get the offset of a netvar given a list of branch names
	* @name:	Top level datatable name
	* @args:	Remaining netvar names
	*
	* Initiate a recursive search down the branch corresponding to the specified datable name
	*/
	template<typename ...args_t>
	int get_offset(const char *name, args_t ...args)
	{
		const auto &node = nodes[name];
		return get_offset_recursive(node->nodes, node->offset, args...);
	}
};