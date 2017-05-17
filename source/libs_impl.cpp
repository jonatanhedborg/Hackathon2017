#define  APP_IMPLEMENTATION
#define  APP_WINDOWS
#include "libs/app.h"

#define ARRAY_IMPLEMENTATION
#include "libs/array.hpp"

#define ASSETSYS_IMPLEMENTATION
#include "libs/assetsys.h"

#define AUDIOSYS_IMPLEMENTATION
#include "libs/audiosys.h"

#define EASE_IMPLEMENTATION
#include "libs/ease.h"

#define FRAMETIMER_IMPLEMENTATION
#include "libs/frametimer.h"

#define GAMESTATE_IMPLEMENTATION
#include "libs/gamestate.hpp"

#define HANDLES_IMPLEMENTATION
#include "libs/handles.h"

#define HASHTABLE_IMPLEMENTATION
#include "libs/hashtable.h"

#define MATH_UTIL_IMPLEMENTATION
#include "libs/math_util.hpp"

#define OBJREPO_IMPLEMENTATION
#include "libs/objrepo.hpp"

#define PALDITHER_IMPLEMENTATION
#include "libs/paldither.h"

#define PALETTIZE_IMPLEMENTATION
#include "libs/palettize.h"

#define RESOURCESYS_IMPLEMENTATION
#include "libs/resources.hpp"

#define RND_IMPLEMENTATION
#include "libs/rnd.h"

#define STRPOOL_IMPLEMENTATION
#include "libs/strpool.h"

#define STRPOOL_HPP_IMPLEMENTATION
#include "libs/strpool.hpp"

#define STRPOOL_UTIL_IMPLEMENTATION
#include "libs/strpool_util.hpp"

#define SYSFONT_IMPLEMENTATION
#include "libs/sysfont.h"

#define THREAD_IMPLEMENTATION
#include "libs/thread.h"

#define TWEEN_IMPLEMENTATION
#include "libs/tween.hpp"

#define VECMATH_IMPLEMENTATION
#include "libs/vecmath.hpp"

#pragma warning( push )
#pragma warning( disable: 4619 ) // there is no warning number 'nnnn'
#pragma warning( disable: 4100 ) // unreferenced formal parameter
#pragma warning( disable: 4127 ) // conditional expression is constant
#pragma warning( disable: 4242 ) // conversion, possible loss of data
#pragma warning( disable: 4244 ) // conversion, possible loss of data
#pragma warning( disable: 4245 ) // conversion, signed/unsigned mismatch
#pragma warning( disable: 4267 ) // '=': conversion from 'size_t' to 'long', possible loss of data
#pragma warning( disable: 4311 ) // 'variable' : pointer truncation from 'type' to 'type'
#pragma warning( disable: 4365 ) // conversion, signed/unsigned mismatch
#pragma warning( disable: 4388 ) // '<': signed/unsigned mismatch
#pragma warning( disable: 4456 ) // declaration hides previous local declaration
#pragma warning( disable: 4457 ) // declaration hides function parameter
#pragma warning( disable: 4459 ) // declaration hides global declaration
#pragma warning( disable: 4505 ) // unreferenced local function has been removed
#pragma warning( disable: 4555 ) // expression has no effect; expected expression with side-effect
#pragma warning( disable: 4701 ) // potentially uninitialized local variable used
#pragma warning( disable: 4706 ) // assignment within conditional expression

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "libs/tinyobj_loader_c.h"

#undef STB_VORBIS_HEADER_ONLY
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_INTEGER_CONVERSION
#include "libs/stb_vorbis.h"

#define STB_IMAGE_IMPLEMENTATION
#pragma push_macro("L")
#undef L
#include "libs/stb_image.h"
#pragma pop_macro("L")
#undef STB_IMAGE_IMPLEMENTATION

#pragma warning( pop )
