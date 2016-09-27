#ifndef TU_CLIENT_ASSETPATH_H
#define TU_CLIENT_ASSETPATH_H

#include <base/math.h>

namespace tu
{

template<int NBTYPES, int NBSRCS, int NBFLAGS, int ID2SIZE=0>
class CGenericPath
{
public:
	int m_Path;
	
	enum
	{
		TYPESIZE = Log2<(NearestPowerOfTwo<NBTYPES>::result)>::result,
		SRCSIZE = Log2<(NearestPowerOfTwo<NBSRCS>::result)>::result,
		FLAGSIZE = Log2<(NearestPowerOfTwo<NBFLAGS>::result)>::result,
		IDSIZE = 32 - FLAGSIZE - SRCSIZE - TYPESIZE - ID2SIZE,
		
		MASK_TYPE = ((0x1 << TYPESIZE)-1),
		MASK_SRC = ((0x1 << SRCSIZE)-1)<<TYPESIZE,
		MASK_FLAGS = ((0x1 << FLAGSIZE)-1)<<(TYPESIZE+SRCSIZE),
		MASK_ID = ((0x1 << IDSIZE)-1)<<(TYPESIZE+SRCSIZE+FLAGSIZE),
		MASK_ID2 = (ID2SIZE > 0 ? ((0x1 << ID2SIZE)-1)<<(TYPESIZE+SRCSIZE+FLAGSIZE+IDSIZE) : 0x0),
		
		TYPE_SHIFT = 0,
		SRC_SHIFT = TYPESIZE,
		FLAGS_SHIFT = TYPESIZE+SRCSIZE,
		ID_SHIFT = TYPESIZE+SRCSIZE+FLAGSIZE,
		ID2_SHIFT = TYPESIZE+SRCSIZE+FLAGSIZE+IDSIZE,
		
		UNDEFINED = -1,
	};
	
public:
	CGenericPath() : m_Path(UNDEFINED) { }
	CGenericPath(int PathInt) : m_Path(PathInt) { }
	CGenericPath(int Type, int Source, int Flags, int Id) : m_Path((Type << TYPE_SHIFT) + (Source << SRC_SHIFT) + (Flags << FLAGS_SHIFT) + (Id << ID_SHIFT)) { }
	CGenericPath(int Type, int Source, int Flags, int Id, int Id2) : m_Path((Type << TYPE_SHIFT) + (Source << SRC_SHIFT) + (Flags << FLAGS_SHIFT) + (Id << ID_SHIFT) + (Id2 << ID2_SHIFT)) { }
	
	operator int() const { return m_Path; }
	
	inline int GetId() const { return (m_Path & MASK_ID) >> ID_SHIFT; }
	inline int GetId2() const { return (m_Path & MASK_ID2) >> ID2_SHIFT; }
	inline int GetType() const { return (m_Path & MASK_TYPE) >> TYPE_SHIFT; }
	inline int GetSource() const { return (m_Path & MASK_SRC) >> SRC_SHIFT; }
	inline int GetFlags() const { return (m_Path & MASK_FLAGS) >> FLAGS_SHIFT; }
	
	inline void SetId(int Id) { m_Path = (m_Path & ~MASK_ID) + (Id << ID_SHIFT); }
	inline void SetId2(int Id) { m_Path = (m_Path & ~MASK_ID2) + (Id << ID2_SHIFT); }
	
	inline bool IsNull() const { return m_Path == UNDEFINED; }
	inline bool operator==(const CGenericPath& path) const { return path.m_Path == m_Path; }
	
	inline void OnIdDeleted(const CGenericPath& Path)
	{
		if(IsNull() || Path.IsNull())
			return;
		
		if((m_Path & ~MASK_ID) != (Path.m_Path & ~MASK_ID))
			return;
		
		int DeletedId = Path.GetId();
		int CurrentId = GetId();
		
		if(CurrentId == DeletedId)
			m_Path = UNDEFINED;
		else if(CurrentId > DeletedId)
			SetId(CurrentId-1);
	}
};

//TAG_NEW_ASSET
#define TU_ASSETTYPE_LIST enum\
	{\
		TYPE_IMAGE = 0,\
		TYPE_SPRITE,\
		TYPE_SKELETON,\
		TYPE_SKELETONANIMATION,\
		TYPE_SKELETONSKIN,\
		TYPE_CHARACTER,\
		TYPE_CHARACTERPART,\
		TYPE_WEAPON,\
		TYPE_MAP,\
		TYPE_MAPGROUP,\
		TYPE_MAPZONETILES,\
		TYPE_MAPLAYERTILES,\
		TYPE_MAPLAYERQUADS,\
		TYPE_ZONETYPE,\
		TYPE_GUIRECTSTYLE,\
		TYPE_GUILINESTYLE,\
		TYPE_GUIBOXSTYLE,\
		TYPE_GUILABELSTYLE,\
		TYPE_GUIBUTTONSTYLE,\
		TYPE_GUITOGGLESTYLE,\
		TYPE_GUISLIDERSTYLE,\
		TYPE_GUISCROLLBARSTYLE,\
		TYPE_GUITABSSTYLE,\
		NUM_ASSETTYPES,\
	};

/*
 * System: Assets that are loaded when the game start and that can't be edited
 */

#define TU_ASSETSOURCE_LIST enum\
	{\
		SRC_SYSTEM=0,\
		SRC_UNIVERSE,\
		SRC_WORLD,\
		SRC_LAND,\
		SRC_SKIN,\
		NUM_SOURCES\
	};

namespace _dontuseit { TU_ASSETSOURCE_LIST }
namespace _dontuseit { TU_ASSETTYPE_LIST }

class CAssetPath : public CGenericPath<_dontuseit::NUM_ASSETTYPES, _dontuseit::NUM_SOURCES, 0>
{
public:
	TU_ASSETSOURCE_LIST
	TU_ASSETTYPE_LIST

public:
	CAssetPath() : CGenericPath() { }
	CAssetPath(int PathInt) : CGenericPath(PathInt) { }
	CAssetPath(int Type, int Source, int Id) : CGenericPath(Type, Source, 0x0, Id) { }
	
	//Static constructors	
	static inline CAssetPath Null() { return CAssetPath(CGenericPath::UNDEFINED); }
	
	static inline CAssetPath Asset(int Type, int Source, int Id) { return CAssetPath(Type, Source, Id); }
	
	static inline CAssetPath System(int Type, int Id) { return Asset(Type, SRC_SYSTEM, Id); }
	static inline CAssetPath Universe(int Type, int Id) { return Asset(Type, SRC_UNIVERSE, Id); }
	static inline CAssetPath World(int Type, int Id) { return Asset(Type, SRC_WORLD, Id); }
	static inline CAssetPath Skin(int Type, int Id) { return Asset(Type, SRC_SKIN, Id); }
	static inline CAssetPath Land(int Type, int Id) { return Asset(Type, SRC_LAND, Id); }
	
	static inline CAssetPath ImageSystem(int Id) { return Asset(TYPE_IMAGE, SRC_SYSTEM, Id); }
	static inline CAssetPath SpriteSystem(int Id) { return Asset(TYPE_SPRITE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiRectStyleSystem(int Id) { return Asset(TYPE_GUIRECTSTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiLineStyleSystem(int Id) { return Asset(TYPE_GUILINESTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiBoxStyleSystem(int Id) { return Asset(TYPE_GUIBOXSTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiLabelStyleSystem(int Id) { return Asset(TYPE_GUILABELSTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiButtonStyleSystem(int Id) { return Asset(TYPE_GUIBUTTONSTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiToggleStyleSystem(int Id) { return Asset(TYPE_GUITOGGLESTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiSliderStyleSystem(int Id) { return Asset(TYPE_GUISLIDERSTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiScrollbarStyleSystem(int Id) { return Asset(TYPE_GUISCROLLBARSTYLE, SRC_SYSTEM, Id); }
	static inline CAssetPath GuiTabsStyleSystem(int Id) { return Asset(TYPE_GUITABSSTYLE, SRC_SYSTEM, Id); }
	
	static inline CAssetPath SpriteUniverse(int Id) { return Asset(TYPE_SPRITE, SRC_UNIVERSE, Id); }
	
	static inline int TypeToStoredType(int Type) { return Type+1; }
	static inline int StoredTypeToType(int StoredType) { return StoredType-1; }
};

//Item position
enum
{
	ITEMLAYER_UNDER_ITEM=0,
	ITEMLAYER_OVER_ITEM,
	ITEMLAYER_OVER_PLAYER,
	ITEMLAYER_UNDER_PARTICULES,
	ITEMLAYER_OVER_PARTICULES,
	ITEMLAYER_UNDER_NAMEPLATES,
	ITEMLAYER_OVER_NAMEPLATES,
	NUM_ITEMLAYER,
	
	ITEMLAYER_PREPLAYER = ITEMLAYER_UNDER_ITEM,
};

//Options
enum
{
	TEXTALIGN_CENTER = 0,
	TEXTALIGN_RIGHT_BOTTOM,
	TEXTALIGN_RIGHT_CENTER,
	TEXTALIGN_RIGHT_TOP,
	TEXTALIGN_CENTER_TOP,
	TEXTALIGN_LEFT_TOP,
	TEXTALIGN_LEFT_CENTER,
	TEXTALIGN_LEFT_BOTTOM,
	TEXTALIGN_CENTER_BOTTOM,
};

/* System *************************************************************/

enum
{
	IMAGE_MENU = 0,
	IMAGE_ASSETSEDITOR,
};

enum
{
	SPRITE_CURSOR=0,
	SPRITE_CURSOR_TRANSLATEX,
	SPRITE_CURSOR_TRANSLATEY,
	SPRITE_CURSOR_TEXT,
	
	SPRITE_MENU_VOID,
	SPRITE_MENU_CHECKBOX_TRUE,
	SPRITE_MENU_CHECKBOX_FALSE,
	SPRITE_MENU_SLIDER_CURSOR,
	
	SPRITE_EDITOR_DEFAULT,
	SPRITE_EDITOR_DECREASE,
	SPRITE_EDITOR_INCREASE,
	SPRITE_EDITOR_DELETE,
	SPRITE_EDITOR_EDIT,
	SPRITE_EDITOR_VIEW,
	SPRITE_EDITOR_UP,
	SPRITE_EDITOR_DOWN,
	SPRITE_EDITOR_DUPLICATE,
	SPRITE_EDITOR_SAVE,
	SPRITE_EDITOR_LOAD,
	SPRITE_EDITOR_FIRST_FRAME,
	SPRITE_EDITOR_PREV_FRAME,
	SPRITE_EDITOR_PLAY,
	SPRITE_EDITOR_PAUSE,
	SPRITE_EDITOR_NEXT_FRAME,
	SPRITE_EDITOR_LAST_FRAME,
	SPRITE_EDITOR_VFLIP,
	SPRITE_EDITOR_HFLIP,
	SPRITE_EDITOR_ROTATE_CW,
	SPRITE_EDITOR_ROTATE_CCW,
	SPRITE_EDITOR_SELECTIONSTAMP,
	SPRITE_EDITOR_SELECTIONFILL,
	SPRITE_EDITOR_GRID,
	SPRITE_EDITOR_NONE,
	SPRITE_EDITOR_FOLDER,
	SPRITE_EDITOR_TILES,
	SPRITE_EDITOR_QUAD,
	SPRITE_EDITOR_WEAPON,
	SPRITE_EDITOR_MAP,
	SPRITE_EDITOR_ZONETILES,
	SPRITE_EDITOR_ZONETYPE,
	SPRITE_EDITOR_ENTITYTYPE,
	SPRITE_EDITOR_ASSET,
	SPRITE_EDITOR_UNIVERSE,
	SPRITE_EDITOR_WORLD,
	SPRITE_EDITOR_LAND,
	SPRITE_EDITOR_SKIN,
	SPRITE_EDITOR_LAYERS,
	SPRITE_EDITOR_BONE,
	SPRITE_EDITOR_SPRITE,
	SPRITE_EDITOR_IMAGE,
	SPRITE_EDITOR_SKELETON,
	SPRITE_EDITOR_SKELETONSKIN,
	SPRITE_EDITOR_SKELETONANIMATION,
	SPRITE_EDITOR_LAYERANIMATION,
	SPRITE_EDITOR_CHARACTER,
	SPRITE_EDITOR_CHARACTERPART,
	SPRITE_EDITOR_MOVE,
	SPRITE_EDITOR_TRANSLATE,
	SPRITE_EDITOR_TRANSLATE_X,
	SPRITE_EDITOR_TRANSLATE_Y,
	SPRITE_EDITOR_ROTATE,
	SPRITE_EDITOR_SCALE,
	SPRITE_EDITOR_SCALE_X,
	SPRITE_EDITOR_SCALE_Y,
	SPRITE_EDITOR_BONE_LENGTH,
	SPRITE_EDITOR_BONE_ADD,
	SPRITE_EDITOR_BONE_DELETE,
	SPRITE_EDITOR_BONE_ATTACH,
	SPRITE_EDITOR_FRAME_MOVE,
	SPRITE_EDITOR_FRAME_ADD,
	SPRITE_EDITOR_FRAME_DELETE,
	SPRITE_EDITOR_FRAMES,
	SPRITE_EDITOR_FRAME_COLOR,
	SPRITE_EDITOR_KEYFRAME_BONE,
	SPRITE_EDITOR_KEYFRAME_LAYER,
	SPRITE_EDITOR_COLORPICKER_RGB,
	SPRITE_EDITOR_COLORPICKER_HSV,
	SPRITE_EDITOR_COLORPICKER_SQUARE,
	SPRITE_EDITOR_COLORPICKER_WHEEL,
	SPRITE_EDITOR_GUIRECT,
	SPRITE_EDITOR_GUILINE,
	SPRITE_EDITOR_GUIBOX,
	SPRITE_EDITOR_GUISCROLLBAR,
	SPRITE_EDITOR_GUITABS,
	SPRITE_EDITOR_SYSTEM,
	SPRITE_EDITOR_GUIBUTTON,
	SPRITE_EDITOR_GUILABEL,
	SPRITE_EDITOR_GUITOGGLE,
	SPRITE_EDITOR_GUISLIDER,
	
	SPRITE_GIZMOFRAME,
	SPRITE_GIZMOFRAME_SELECT,
	SPRITE_GIZMOFRAME_ADD,
	SPRITE_GIZMOFRAME_DELETE,
	SPRITE_GIZMOFRAME_COLOR,
	SPRITE_GIZMOFRAME_HIDDEN,
	SPRITE_GIZMOFRAME_COLORED_BG,
	SPRITE_GIZMOFRAME_COLORED_FG,
	
	SPRITE_GIZMO_VERTEX_BG,
	SPRITE_GIZMO_VERTEX_FG,
	SPRITE_GIZMO_PIVOT,
	SPRITE_GIZMO_SCALE,
	SPRITE_GIZMO_ROTATE,
	
	SPRITE_GIZMODIR_AIM,
	SPRITE_GIZMODIR_MOVE,
	SPRITE_GIZMODIR_HOOK,
};

enum
{
	SKELETON_PLACEHOLDER = 0,
};

enum
{
	PLACEHOLDERBONE_ROOT = 0,
};

enum
{
	PLACEHOLDERLAYER_COLOR = 0,
};

enum
{
	GUIRECTSTYLE_COMPOSING=0,
	GUIRECTSTYLE_SELECTION,
	GUIRECTSTYLE_STAMP,
	GUIRECTSTYLE_MENU_BACKGROUND,
	GUIRECTSTYLE_MENU_MOUSEOVER,
	GUIRECTSTYLE_MENU_HIGHLIGHT,
	GUIRECTSTYLE_EDITOR_BUTTON,
	GUIRECTSTYLE_EDITOR_BUTTON_MO,
	GUIRECTSTYLE_EDITOR_BUTTON_HL,
	GUIRECTSTYLE_EDITOR_ENTRY,
	GUIRECTSTYLE_EDITOR_ENTRY_MO,
	GUIRECTSTYLE_EDITOR_POPUP,
	GUIRECTSTYLE_EDITOR_TABS_LAYOUT,
	GUIRECTSTYLE_EDITOR_TABS_CONTENT,
	GUIRECTSTYLE_EDITOR_TABS_INACTIVEBUTTON,
	GUIRECTSTYLE_EDITOR_TABS_ACTIVEBUTTON,
	GUIRECTSTYLE_EDITOR_VIEW,
};

enum
{
	GUILINESTYLE_SEPARATOR=0,
	GUILINESTYLE_MENU_SLIDER_RAIL,
	GUILINESTYLE_MENU_SCROLLBAR_RAIL,
	GUILINESTYLE_MENU_SCROLLBAR_SLIDER,
	GUILINESTYLE_EDITOR_SCROLLBAR_RAIL,
	GUILINESTYLE_EDITOR_SCROLLBAR_SLIDER,
};

enum
{
	GUIBOXSTYLE_MENU_COLUMNS=0,
	GUIBOXSTYLE_MENU_SECTIONLIST,
	GUIBOXSTYLE_MENU_SECTION,
	GUIBOXSTYLE_MENU_LIST,
	GUIBOXSTYLE_MENU_MAINTABS_LAYOUT,
	GUIBOXSTYLE_MENU_MAINTABS_BUTTONLIST,
	GUIBOXSTYLE_MENU_ITEM,
	GUIBOXSTYLE_EDITOR_BUTTON_HL,
	GUIBOXSTYLE_EDITOR_TEXTENTRY,
	GUIBOXSTYLE_EDITOR_NUMERICENTRY,
	GUIBOXSTYLE_EDITOR_POPUP,
	GUIBOXSTYLE_EDITOR_ASSETEDITBUTTON,
	GUIBOXSTYLE_EDITOR_TABS_LAYOUT,
	GUIBOXSTYLE_EDITOR_TABS_CONTENT,
	GUIBOXSTYLE_EDITOR_TABS_BUTTONLIST,
	GUIBOXSTYLE_EDITOR_VIEW,
};

enum
{
	GUILABELSTYLE_COMPOSING=0,
	GUILABELSTYLE_MENU_TEXT,
	GUILABELSTYLE_MENU_HEADER,
	GUILABELSTYLE_MENU_BUTTON_IDLE,
	GUILABELSTYLE_MENU_BUTTON_MO,
	GUILABELSTYLE_MENU_MAINTABS_INACTIVEBUTTON,
	GUILABELSTYLE_MENU_MAINTABS_INACTIVEBUTTON_MO,
	GUILABELSTYLE_MENU_MAINTABS_ACTIVEBUTTON,
	GUILABELSTYLE_EDITOR_TEXT,
	GUILABELSTYLE_EDITOR_HEADER,
	GUILABELSTYLE_EDITOR_BUTTON_IDLE,
	GUILABELSTYLE_EDITOR_BUTTON_MO,
	GUILABELSTYLE_EDITOR_ACTIVETAB,
	GUILABELSTYLE_EDITOR_INACTIVETAB,
	GUILABELSTYLE_EDITOR_LISTITEM,
	GUILABELSTYLE_EDITOR_LISTITEM_MO,
	GUILABELSTYLE_EDITOR_INACTIVELISTITEM,
};

enum
{
	GUIBUTTONSTYLE_MENU_DEFAULT=0,
	GUIBUTTONSTYLE_MENU_MAINTABS_INACTIVEBUTTON,
	GUIBUTTONSTYLE_MENU_MAINTABS_ACTIVEBUTTON,
	GUIBUTTONSTYLE_EDITOR_DEFAULT,
	GUIBUTTONSTYLE_EDITOR_ACTIVETAB,
	GUIBUTTONSTYLE_EDITOR_INACTIVETAB,
	GUIBUTTONSTYLE_EDITOR_LISTITEM,
};

enum
{
	GUITOGGLESTYLE_MENU_CHECKBOX=0,
};

enum
{
	GUISLIDERSTYLE_MENU_DEFAULT=0,
};

enum
{
	GUISCROLLBARSTYLE_MENU=0,
	GUISCROLLBARSTYLE_EDITOR,
};

enum
{
	GUITABSSTYLE_MENU_MAIN=0,
	GUITABSSTYLE_EDITOR,
};

/* Universe ***********************************************************/
enum
{
	IMAGE_GAME = 0,
	IMAGE_PARTICLES,
	IMAGE_TEEBODYDEFAULT,
	IMAGE_TEEFEETDEFAULT,
	IMAGE_TEEHANDSDEFAULT,
	IMAGE_TEEEYESDEFAULT,
};

enum
{
	SPRITE_WHITESQUARE = 0,
	SPRITE_HAMMER,
	SPRITE_GUN,
	SPRITE_SHOTGUN,
	SPRITE_GRENADE,
	SPRITE_LASER,
	SPRITE_NINJA,
	SPRITE_GUN_MUZZLE1,
	SPRITE_GUN_MUZZLE2,
	SPRITE_GUN_MUZZLE3,
	SPRITE_SHOTGUN_MUZZLE1,
	SPRITE_SHOTGUN_MUZZLE2,
	SPRITE_SHOTGUN_MUZZLE3,
	SPRITE_NINJA_MUZZLE1,
	SPRITE_NINJA_MUZZLE2,
	SPRITE_NINJA_MUZZLE3,
	SPRITE_HAMMER_CURSOR,
	SPRITE_GUN_CURSOR,
	SPRITE_SHOTGUN_CURSOR,
	SPRITE_GRENADE_CURSOR,
	SPRITE_LASER_CURSOR,
	SPRITE_NINJA_CURSOR,
	SPRITE_HOOK_HEAD,
	SPRITE_HOOK_CHAIN,
	
	SPRITE_PART_SPLAT1,
	SPRITE_PART_SPLAT2,
	SPRITE_PART_SPLAT3,
	
	SPRITE_TEEBODYDEFAULT_SHADOW,
	SPRITE_TEEBODYDEFAULT_COLOR,
	SPRITE_TEEBODYDEFAULT_SHADING,
	SPRITE_TEEBODYDEFAULT_OUTLINE,
	
	SPRITE_TEEFEETDEFAULT_SHADOW,
	SPRITE_TEEFEETDEFAULT_COLOR,
	
	SPRITE_TEEHANDSDEFAULT_SHADOW,
	SPRITE_TEEHANDSDEFAULT_COLOR,
	
	SPRITE_TEEEYESDEFAULT_NORMAL,
	SPRITE_TEEEYESDEFAULT_ANGRY,
	SPRITE_TEEEYESDEFAULT_PAIN,
	SPRITE_TEEEYESDEFAULT_HAPPY,
	SPRITE_TEEEYESDEFAULT_FEAR,
};

enum
{
	LIST_GUN_MUZZLES = 0,
	LIST_SHOTGUN_MUZZLES,
	LIST_NINJA_MUZZLES,
	LIST_PART_SPLATS,
};

enum
{
	SKELETON_TEE=0,
	SKELETON_HAMMER,
	SKELETON_GUN,
	SKELETON_SHOTGUN,
};

enum
{
	TEEBONE_BODY = 0,
	TEEBONE_EYES,
	TEEBONE_BACKFOOT,
	TEEBONE_FRONTFOOT,
	TEEBONE_BACKARM,
	TEEBONE_BACKHAND,
	TEEBONE_FRONTARM,
	TEEBONE_FRONTHAND,
};

enum
{
	TEELAYER_ATTACH = 0,
	TEELAYER_BACKHANDSHADOW,
	TEELAYER_FRONTHANDSHADOW,
	TEELAYER_BACKFOOTSHADOW,
	TEELAYER_BODYSHADOW,
	TEELAYER_FRONTFOOTSHADOW,
	TEELAYER_BACKHAND,
	TEELAYER_FRONTHAND,
	TEELAYER_BACKFOOT,
	TEELAYER_DECORATION,
	TEELAYER_BODY,
	TEELAYER_MARKING,
	TEELAYER_OUTLINE,
	TEELAYER_EYES_NORMAL,
	TEELAYER_EYES_ANGRY,
	TEELAYER_EYES_PAIN,
	TEELAYER_EYES_HAPPY,
	TEELAYER_EYES_FEAR,
	TEELAYER_FRONTFOOT,
};

enum
{
	SKELETONSKIN_TEEBODYDEFAULT = 0,
	SKELETONSKIN_TEEFEETDEFAULT,
	SKELETONSKIN_TEEHANDSDEFAULT,
	SKELETONSKIN_TEEEYESDEFAULT,
	SKELETONSKIN_HAMMER,
	SKELETONSKIN_GUN,
	SKELETONSKIN_SHOTGUN,
};

enum
{
	SKELETONANIMATION_TEEIDLE = 0,
	SKELETONANIMATION_TEEAIR,
	SKELETONANIMATION_TEEAIR2,
	SKELETONANIMATION_TEEWALK,
	SKELETONANIMATION_HAMMERATTACK,
	SKELETONANIMATION_GUNATTACK,
	SKELETONANIMATION_SHOTGUNATTACK,
};

enum
{
	SKINPART_BODY=0,
	SKINPART_MARKING,
	SKINPART_DECORATION,
	SKINPART_HANDS,
	SKINPART_FEET,
	SKINPART_EYES,
};

enum
{
	CHARACTER_TEE=0,
};

enum
{
	CHARACTERPART_TEEBODYDEFAULT=0,
	CHARACTERPART_TEEFEETDEFAULT,
	CHARACTERPART_TEEHANDSDEFAULT,
	CHARACTERPART_TEEEYESDEFAULT,
};

enum
{
	WEAPON_HAMMER=0,
	WEAPON_GUN,
};

enum
{
	ZONETYPE_PHYSICS=0,
	ZONETYPE_DEATH,
};

enum
{
	ZONETYPEINDEX_PHYSICS_AIR=0,
	ZONETYPEINDEX_PHYSICS_GROUND=1,
	ZONETYPEINDEX_DEATH_DEATH=2,
	ZONETYPEINDEX_PHYSICS_NOHOOK=3,
};

}

#endif