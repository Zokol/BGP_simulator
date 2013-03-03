import os

TRACK_USAGE = False

PROTOCOL_VERSION = '0.4'

BASEDIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if BASEDIR.endswith('.zip'): # Because py2exe, that's why (and deadlines)
	BASEDIR = os.path.dirname(BASEDIR)
GFXDIR = os.path.join(BASEDIR, 'gfx')
SNDDIR = os.path.join(BASEDIR, 'snd')
MAPDIR = os.path.join(BASEDIR, 'map')
FONTDIR = os.path.join(BASEDIR, 'font')

FONT = os.path.join(FONTDIR, 'FreeSansBold.ttf')
FONTSCALE = 0.6875

ICON_BORDER = 2
ICON_RADIUS = 9
ICON_FONTSIZE = 15
ICON_MARGIN = 2
ICON_PADDING = 4

COLOR_FONT = (0, 0, 0)
COLOR_BG = (159, 182, 205)
COLOR_BORDER = (50, 50, 50)
COLOR_SELECTED = (255,255,0)

ORIG_TILE_SIZE = (24, 24)
ORIG_CHAR_SIZE = (24, 32)
ORIG_HAIR_SIZE = (22, 22)
ORIG_ARMOR_SIZE = (24, 24)
ORIG_BORDER_SIZE = (8, 8)
ORIG_OVERLAY_SIZE = (24, 32)

SCALE = 2
TILE_SIZE = (SCALE * ORIG_TILE_SIZE[0], SCALE * ORIG_TILE_SIZE[1])
CHAR_SIZE = (SCALE * ORIG_CHAR_SIZE[0], SCALE * ORIG_CHAR_SIZE[1])
HAIR_SIZE = (SCALE * ORIG_HAIR_SIZE[0], SCALE * ORIG_HAIR_SIZE[1])
ARMOR_SIZE = (SCALE * ORIG_ARMOR_SIZE[0], SCALE * ORIG_ARMOR_SIZE[1])
BORDER_SIZE = (SCALE * ORIG_BORDER_SIZE[0], SCALE * ORIG_BORDER_SIZE[1])
OVERLAY_SIZE = (SCALE * ORIG_OVERLAY_SIZE[0], SCALE * ORIG_OVERLAY_SIZE[1])

FPS = 30
HIT_FPS = 10
TILE_FPS = 2

BGM_FADE_MS = 500

# XXX: Trees don't occlude characters below them since drawing selections would be a pain in the ass.
#      If we do it, trees occlude selections too...
L_MAP =          lambda y: (0, y)
L_SEL =          lambda y: (1, )
L_CHAR =         lambda y: (2, y)
L_CHAR_OVERLAY = lambda y: (2, y, 0)
L_CHAR_EFFECT =  lambda y: (2, y, 1)
L_GAMEOVER =     (3, )

RACE_SPRITES = {"dwarf": ("races.png", 1)
	,"human": ("races.png", 2)
	,"thiefling": ("races.png", 3)
	,"gnome": ("races.png", 4)
	,"elf": ("races.png", 5)
	,"goblin": ("races.png", 6)
	,"vampire": ("races.png", 7)
	,"djinn": ("races.png", 8)
	,"imp": ("races.png", 9)
	,"minotaur": ("races.png", 10)
	,"mindflayer": ("races.png", 11)
	,"ogre": ("races.png", 12)
	,"mummy": ("races.png", 13)
	,"werewolf": ("races.png", 14)
	,"orc": ("races.png", 15)}

HAIR_SPRITES = {"a": ("hair_collection_colorkey_fixed.png", 1)
	,"b": ("hair_collection_colorkey_fixed.png", 2)
	,"c": ("hair_collection_colorkey_fixed.png", 3)
	,"d": ("hair_collection_colorkey_fixed.png", 4)
	,"e": ("hair_collection_colorkey_fixed.png", 5)
	,"f": ("hair_collection_colorkey_fixed.png", 6)
	,"g": ("hair_collection_colorkey_fixed.png", 7)
	,"h": ("hair_collection_colorkey_fixed.png", 8)
	,"i": ("hair_collection_colorkey_fixed.png", 9)
	,"j": ("hair_collection_colorkey_fixed.png", 10)}

RACE_STATS = {"Human": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Human2": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Devil": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Human3": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Elf": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Alien": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"WhiteGuy": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Medusa": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Dragon": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Taurus": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Squid": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"GreyGuy": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Imhotep": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]
	,"Wolf": [("Health", 100), ("Defence", 80), ("Attack", 20), ("Dexterity", 40)]}
	
RACE_HAIRS = {"a": [("Human", 1), ("Human2", 0), ("Human3", 0)]
	,"b": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"c": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"d": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"e": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"f": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"g": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"h": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"i": [("Human", 0), ("Human2", 0), ("Human3", 0)]
	,"j": [("Human", 0), ("Human2", 0), ("Human3", 0)]}
