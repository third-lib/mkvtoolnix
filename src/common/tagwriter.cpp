/*
 * mkvextract -- extract tracks from Matroska files into other files
 *
 * Distributed under the GPL
 * see the file COPYING for details
 * or visit http://www.gnu.org/copyleft/gpl.html
 *
 * $Id$
 *
 * writes tags in XML format
 *
 * Written by Moritz Bunkus <moritz@bunkus.org>.
 */

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include <matroska/KaxTag.h>
#include <matroska/KaxTags.h>
#include <matroska/KaxTagMulti.h>

using namespace libmatroska;
using namespace std;

#include "base64.h"
#include "common.h"
#include "commonebml.h"
#include "matroska.h"
#include "mm_io.h"
#include "xml_element_mapping.h"

static mm_io_c *o;

// static void
// print_tag(int level,
//           const char *name,
//           const char *fmt,
//           ...) {
//   int idx;
//   va_list ap;
//   string new_fmt;

//   fix_format(fmt, new_fmt);
//   for (idx = 0; idx < level; idx++)
//     mxprint(o, "  ");
//   mxprint(o, "<%s>", name);
//   va_start(ap, fmt);
//   vfprintf(o, fmt, ap);
//   va_end(ap);
//   mxprint(o, "</%s>\n", name);
// }

// static void
// print_binary(int level,
//              const char *name,
//              EbmlElement *e) {
//   EbmlBinary *b;
//   string s;
//   int i, idx, old_idx;

//   b = (EbmlBinary *)e;
//   s = base64_encode((const unsigned char *)b->GetBuffer(), b->GetSize(), true,
//                     72 - level - 2);
//   if (s[s.length() - 1] == '\n')
//     s.erase(s.length() - 1);

//   for (i = 0; i < level; i++)
//     mxprint(o, "  ");

//   if ((level * 2 + 2 * strlen(name) + 2 + 3 + s.length()) <= 78) {
//     mxprint(o, "<%s>%s</%s>\n", name, s.c_str(), name);
//     return;
//   }

//   mxprint(o, "<%s>\n", name);

//   for (i = 0; i < (level + 2); i++)
//     mxprint(o, "  ");

//   old_idx = 0;
//   for (idx = 0; idx < s.length(); idx++)
//     if (s[idx] == '\n') {
//       mxprint(o, "%s\n", s.substr(old_idx, idx - old_idx).c_str());
//       for (i = 0; i < (level + 2); i++)
//         mxprint(o, "  ");
//       old_idx = idx + 1;
//     }

//   if (old_idx < s.length())
//     mxprint(o, "%s", s.substr(old_idx).c_str());

//   mxprint(o, "\n");

//   for (i = 0; i < level; i++)
//     mxprint(o, "  ");
//   mxprint(o, "</%s>\n", name);
// }

// static void
// print_date(int level,
//            const char *name,
//            EbmlElement *e) {
//   int idx;
//   time_t tme;
//   struct tm *tm;
//   char buffer[100];

//   for (idx = 0; idx < level; idx++)
//     mxprint(o, "  ");
//   mxprint(o, "<%s>", name);

//   tme = ((EbmlDate *)e)->GetEpochDate();
//   tm = gmtime(&tme);
//   if (tm == NULL)
//     mxprint(o, "INVALID: %llu", (uint64_t)tme);
//   else {
//     buffer[99] = 0;
//     strftime(buffer, 99, "%Y-%m-%dT%H:%M:%S+0000", tm);
//     mxprint(o, buffer);
//   }
  
//   mxprint(o, "</%s>\n", name);
// }

// static void
// print_string(int level,
//              const char *name,
//              const char *src) {
//   int idx;
//   string s;

//   idx = 0;
//   s = "";
//   while (src[idx] != 0) {
//     if (src[idx] == '&')
//       s += "&amp;";
//     else if (src[idx] == '<')
//       s += "&lt;";
//     else if (src[idx] == '>')
//       s += "&gt;";
//     else if (src[idx] == '"')
//       s += "&quot;";
//     else
//       s += src[idx];
//     idx++;
//   }
//   for (idx = 0; idx < level; idx++)
//     mxprint(o, "  ");
//   mxprint(o, "<%s>%s</%s>\n", name, s.c_str(), name);
// }

// static void
// print_utf_string(int level,
//                  const char *name,
//                  EbmlElement *e) {
//   char *s;

//   s = UTFstring_to_cstrutf8(*static_cast<EbmlUnicodeString *>(e));
//   print_string(level, name, s);
//   safefree(s);
// }

// static void
// print_unknown(int level,
//               EbmlElement *e) {
//   int idx;

//   for (idx = 0; idx < level; idx++)
//     mxprint(o, "  ");
//   mxprint(o, "<!-- Unknown element: %s -->\n", e->Generic().DebugName);
// }

// #define pr_ui(n) print_tag(level, n, "%llu", \
//                            uint64(*static_cast<EbmlUInteger *>(e)))
// #define pr_si(n) print_tag(level, n, "%lld", \
//                            int64(*static_cast<EbmlSInteger *>(e)))
// #define pr_f(n) print_tag(level, n, "%f", \
//                           float(*static_cast<EbmlFloat *>(e)))
// #define pr_s(n) print_string(level, n, \
//                              string(*static_cast<EbmlString *>(e)).c_str())
// #define pr_us(n) print_utf_string(level, n, e)
// #define pr_d(n) print_date(level, n, e)

// #define pr_b(n) print_binary(level, n, e)

// #define pr_unk() print_unknown(level, e)

// static void
// handle_multicomments(EbmlElement *e,
//                      int level) {
//   if (is_id(e, KaxTagMultiCommentName))
//     pr_s("Name");

//   else if (is_id(e, KaxTagMultiCommentComments))
//     pr_us("Comments");

//   else if (is_id(e, KaxTagMultiCommentLanguage))
//     pr_s("Language");

//   else
//     pr_unk();
// }

// static void
// handle_simpletags(EbmlElement *e,
//                   int level) {
//   int i;

//   if (is_id(e, KaxTagName))
//     pr_us("Name");

//   else if (is_id(e, KaxTagString))
//     pr_us("String");

//   else if (is_id(e, KaxTagBinary))
//     pr_b("Binary");

//   else if (is_id(e, KaxTagLangue))
//     pr_s("TagLanguage");

//   else if (is_id(e, KaxTagDefault))
//     pr_ui("DefaultLanguage");

//   else if (is_id(e, KaxTagSimple)) {
//     for (i = 0; i < level; i++)
//       mxprint(o, "  ");
//     mxprint(o, "<Simple>\n");
//     for (i = 0; i < (int)((EbmlMaster *)e)->ListSize(); i++)
//       handle_simpletags((*((EbmlMaster *)e))[i], level + 1);
//     for (i = 0; i < level; i++)
//       mxprint(o, "  ");
//     mxprint(o, "</Simple>\n");

//   } else
//     pr_unk();
// }

// static void
// handle_level5(EbmlElement *e) {
//   int i, level = 5;

//   if (is_id(e, KaxTagMultiPriceCurrency))
//     pr_s("Currency");

//   else if (is_id(e, KaxTagMultiPriceAmount))
//     pr_f("Amount");

//   else if (is_id(e, KaxTagMultiPricePriceDate))
//     pr_d("PriceDate");

//   else if (is_id(e, KaxTagMultiComment)) {
//     mxprint(o, "          <MultiComment>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_multicomments((*(EbmlMaster *)e)[i], 6);
//     mxprint(o, "          </MultiComment>\n");
//   }

//   else
//     pr_unk();
// }

// static void
// handle_level4(EbmlElement *e) {
//   int i, level = 4;

//   if (is_id(e, KaxTagMultiCommercialType))
//     pr_ui("CommercialType");

//   else if (is_id(e, KaxTagMultiCommercialAddress))
//     pr_us("Address");

//   else if (is_id(e, KaxTagMultiCommercialURL))
//     pr_s("URL");

//   else if (is_id(e, KaxTagMultiCommercialEmail))
//     pr_s("Email");

//   else if (is_id(e, KaxTagMultiPrice)) {
//     mxprint(o, "        <MultiPrice>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level5((*(EbmlMaster *)e)[i]);
//     mxprint(o, "        </MultiPrice>\n");

//   } else if (is_id(e, KaxTagMultiDateType))
//     pr_ui("DateType");

//   else if (is_id(e, KaxTagMultiDateDateBegin))
//     pr_d("Begin");

//   else if (is_id(e, KaxTagMultiDateDateEnd))
//     pr_d("End");

//   else if (is_id(e, KaxTagMultiEntityType))
//     pr_ui("EntityType");

//   else if (is_id(e, KaxTagMultiEntityName))
//     pr_us("Name");

//   else if (is_id(e, KaxTagMultiEntityURL))
//     pr_s("URL");

//   else if (is_id(e, KaxTagMultiEntityEmail))
//     pr_s("Email");
  
//   else if (is_id(e, KaxTagMultiEntityAddress))
//     pr_us("Address");

//   else if (is_id(e, KaxTagMultiIdentifierType))
//     pr_ui("IdentifierType");

//   else if (is_id(e, KaxTagMultiIdentifierBinary))
//     pr_b("Binary");

//   else if (is_id(e, KaxTagMultiIdentifierString))
//     pr_us("String");

//   else if (is_id(e, KaxTagMultiLegalType))
//     pr_ui("LegalType");

//   else if (is_id(e, KaxTagMultiLegalURL))
//     pr_s("URL");

//   else if (is_id(e, KaxTagMultiLegalAddress))
//     pr_us("Address");
 
//   else if (is_id(e, KaxTagMultiLegalContent))
//     pr_us("Content");

//   else if (is_id(e, KaxTagMultiTitleType))
//     pr_ui("TitleType");

//   else if (is_id(e, KaxTagMultiTitleName))
//     pr_us("Name");

//   else if (is_id(e, KaxTagMultiTitleSubTitle))
//     pr_us("SubTitle");

//   else if (is_id(e, KaxTagMultiTitleEdition))
//     pr_us("Edition");

//   else if (is_id(e, KaxTagMultiTitleAddress))
//     pr_us("Address");

//   else if (is_id(e, KaxTagMultiTitleURL))
//     pr_s("URL");

//   else if (is_id(e, KaxTagMultiTitleEmail))
//     pr_s("Email");

//   else if (is_id(e, KaxTagMultiTitleLanguage))
//     pr_s("Language");

//   else if (is_id(e, KaxTagMultiComment)) {
//     mxprint(o, "        <MultiComment>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_multicomments((*(EbmlMaster *)e)[i], 5);
//     mxprint(o, "        </MultiComment>\n");
//   }

//   else
//     pr_unk();
// }

// static void
// handle_level3(EbmlElement *e) {
//   int i, level = 3;

//   if (is_id(e, KaxTagTrackUID))
//     pr_ui("TrackUID");

//   else if (is_id(e, KaxTagChapterUID))
//     pr_ui("ChapterUID");

//   else if (is_id(e, KaxTagEditionUID))
//     pr_ui("EditionUID");

//   else if (is_id(e, KaxTagTargetType))
//     pr_s("TargetType");

//   else if (is_id(e, KaxTagSubject))
//     pr_us("Subject");

//   else if (is_id(e, KaxTagBibliography))
//     pr_us("Bibliography");

//   else if (is_id(e, KaxTagLanguage))
//     pr_s("Language");

//   else if (is_id(e, KaxTagRating))
//     pr_b("Rating");

//   else if (is_id(e, KaxTagEncoder))
//     pr_us("Encoder");

//   else if (is_id(e, KaxTagEncodeSettings))
//     pr_us("EncodeSettings");

//   else if (is_id(e, KaxTagFile))
//     pr_us("File");

//   else if (is_id(e, KaxTagArchivalLocation))
//     pr_us("ArchivalLocation");

//   else if (is_id(e, KaxTagKeywords))
//     pr_us("Keywords");

//   else if (is_id(e, KaxTagMood))
//     pr_us("Mood");

//   else if (is_id(e, KaxTagRecordLocation))
//     pr_s("RecordLocation");

//   else if (is_id(e, KaxTagSource))
//     pr_us("Source");

//   else if (is_id(e, KaxTagSourceForm))
//     pr_us("SourceForm");

//   else if (is_id(e, KaxTagProduct))
//     pr_us("Product");

//   else if (is_id(e, KaxTagOriginalMediaType))
//     pr_us("OriginalMediaType");

//   else if (is_id(e, KaxTagPlayCounter))
//     pr_ui("PlayCounter");

//   else if (is_id(e, KaxTagPopularimeter))
//     pr_ui("Popularimeter");

//   else if (is_id(e, KaxTagAudioGenre))
//     pr_s("AudioGenre");

//   else if (is_id(e, KaxTagVideoGenre))
//     pr_b("VideoGenre");

//   else if (is_id(e, KaxTagSubGenre))
//     pr_s("SubGenre");

//   else if (is_id(e, KaxTagAudioEncryption))
//     pr_b("AudioEncryption");

//   else if (is_id(e, KaxTagAudioGain))
//     pr_f("AudioGain");

//   else if (is_id(e, KaxTagAudioPeak))
//     pr_f("AudioPeak");

//   else if (is_id(e, KaxTagBPM))
//     pr_f("BPM");

//   else if (is_id(e, KaxTagEqualisation))
//     pr_b("Equalisation");

//   else if (is_id(e, KaxTagDiscTrack))
//     pr_ui("DiscTrack");

//   else if (is_id(e, KaxTagSetPart))
//     pr_ui("SetPart");

//   else if (is_id(e, KaxTagInitialKey))
//     pr_s("InitialKey");

//   else if (is_id(e, KaxTagOfficialAudioFileURL))
//     pr_s("OfficialAudioFileURL");

//   else if (is_id(e, KaxTagOfficialAudioSourceURL))
//     pr_s("OfficialAudioSourceURL");

//   else if (is_id(e, KaxTagCaptureDPI))
//     pr_ui("CaptureDPI");

//   else if (is_id(e, KaxTagCaptureLightness))
//     pr_b("CaptureLightness");

//   else if (is_id(e, KaxTagCapturePaletteSetting))
//     pr_ui("CapturePaletteSetting");

//   else if (is_id(e, KaxTagCaptureSharpness))
//     pr_b("CaptureSharpness");

//   else if (is_id(e, KaxTagCropped))
//     pr_us("Cropped");

//   else if (is_id(e, KaxTagOriginalDimensions))
//     pr_s("OriginalDimensions");

//   else if (is_id(e, KaxTagCommercial)) {
//     mxprint(o, "      <Commercial>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level4((*(EbmlMaster *)e)[i]);
//     mxprint(o, "      </Commercial>\n");

//   } else if (is_id(e, KaxTagDate)) {
//     mxprint(o, "      <Date>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level4((*(EbmlMaster *)e)[i]);
//     mxprint(o, "      </Date>\n");

//   } else if (is_id(e, KaxTagEntity)) {
//     mxprint(o, "      <Entity>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level4((*(EbmlMaster *)e)[i]);
//     mxprint(o, "      </Entity>\n");

//   } else if (is_id(e, KaxTagIdentifier)) {
//     mxprint(o, "      <Identifier>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level4((*(EbmlMaster *)e)[i]);
//     mxprint(o, "      </Identifier>\n");

//   } else if (is_id(e, KaxTagLegal)) {
//     mxprint(o, "      <Legal>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level4((*(EbmlMaster *)e)[i]);
//     mxprint(o, "      </Legal>\n");

//   } else if (is_id(e, KaxTagTitle)) {
//     mxprint(o, "      <Title>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level4((*(EbmlMaster *)e)[i]);
//     mxprint(o, "      </Title>\n");

//   } else if (is_id(e, KaxTagMultiComment)) {
//     mxprint(o, "      <MultiComment>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_multicomments((*(EbmlMaster *)e)[i], 4);
//     mxprint(o, "      </MultiComment>\n");

//   } else
//     pr_unk();
// }

// static void
// handle_level2(EbmlElement *e) {
//   int i, level = 2;

//   if (is_id(e, KaxTagTargets)) {
//     mxprint(o, "    <Targets>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </Targets>\n");

//   } else if (is_id(e, KaxTagGeneral)) {
//     mxprint(o, "    <General>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </General>\n");

//   } else if (is_id(e, KaxTagGenres)) {
//     mxprint(o, "    <Genres>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </Genres>\n");

//   } else if (is_id(e, KaxTagAudioSpecific)) {
//     mxprint(o, "    <AudioSpecific>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </AudioSpecific>\n");

//   } else if (is_id(e, KaxTagImageSpecific)) {
//     mxprint(o, "    <ImageSpecific>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </ImageSpecific>\n");

//   } else if (is_id(e, KaxTagMultiCommercial)) {
//     mxprint(o, "    <MultiCommercial>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </MultiCommercial>\n");

//   } else if (is_id(e, KaxTagMultiDate)) {
//     mxprint(o, "    <MultiDate>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </MultiDate>\n");

//   } else if (is_id(e, KaxTagMultiEntity)) {
//     mxprint(o, "    <MultiEntity>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </MultiEntity>\n");

//   } else if (is_id(e, KaxTagMultiIdentifier)) {
//     mxprint(o, "    <MultiIdentifier>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </MultiIdentifier>\n");

//   } else if (is_id(e, KaxTagMultiLegal)) {
//     mxprint(o, "    <MultiLegal>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </MultiLegal>\n");

//   } else if (is_id(e, KaxTagMultiTitle)) {
//     mxprint(o, "    <MultiTitle>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level3((*(EbmlMaster *)e)[i]);
//     mxprint(o, "    </MultiTitle>\n");

//   } else if (is_id(e, KaxTagMultiComment)) {
//     mxprint(o, "    <MultiComment>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_multicomments((*(EbmlMaster *)e)[i], 3);
//     mxprint(o, "    </MultiComment>\n");

//   } else if (is_id(e, KaxTagSimple)) {
//     mxprint(o, "    <Simple>\n");
//     for (i = 0; i < (int)((EbmlMaster *)e)->ListSize(); i++)
//       handle_simpletags((*((EbmlMaster *)e))[i], level + 1);
//     mxprint(o, "    </Simple>\n");

//   } else
//     pr_unk();
// }

// static void
// handle_level1(EbmlElement *e) {
//   int i, level = 1;

//   if (is_id(e, KaxTag)) {
//     mxprint(o, "  <Tag>\n");
//     for (i = 0; i < ((EbmlMaster *)e)->ListSize(); i++)
//       handle_level2((*(EbmlMaster *)e)[i]);
//     mxprint(o, "  </Tag>\n");
//   } else
//     pr_unk();

// }

typedef struct {
  int level;
  int parent_idx;
  int elt_idx;
  EbmlElement *e;
} tag_writer_cb_t;

static void
pt(int level,
   const char *tag) {
  int i;

  for (i = 0; i < level; i++)
    o->printf("  ");
  o->printf("%s", tag);
}

static void
write_xml_element_rec(int level,
                      int parent_idx,
                      EbmlElement *e) {
  EbmlMaster *m;
  int elt_idx, i;
  bool found;
  char *s;

  elt_idx = parent_idx;
  found = false;
  while ((tag_elements[elt_idx].name != NULL) &&
         (tag_elements[elt_idx].level >=
          tag_elements[parent_idx].level)) {
    if (tag_elements[elt_idx].id == e->Generic().GlobalId) {
      found = true;
      break;
    }
    elt_idx++;
  }

  for (i = 0; i < level; i++)
    o->printf("  ");

  if (!found) {
    o->printf("<!-- Unknown element '%s' -->\n", e->Generic().DebugName);
    return;
  }

  o->printf("<%s>", tag_elements[elt_idx].name);
  switch (tag_elements[elt_idx].type) {
    case ebmlt_master:
      o->printf("\n");
      m = dynamic_cast<EbmlMaster *>(e);
      assert(m != NULL);
      for (i = 0; i < m->ListSize(); i++)
        write_xml_element_rec(level + 1, elt_idx, (*m)[i]);

      if (tag_elements[elt_idx].end_hook != NULL) {
        tag_writer_cb_t cb;

        cb.level = level;
        cb.parent_idx = parent_idx;
        cb.elt_idx = elt_idx;
        cb.e = e;

        tag_elements[elt_idx].end_hook(&cb);
      }

      for (i = 0; i < level; i++)
        o->printf("  ");
      o->printf("</%s>\n", tag_elements[elt_idx].name);
      break;

    case ebmlt_uint:
    case ebmlt_bool:
      o->printf("%llu</%s>\n", uint64(*dynamic_cast<EbmlUInteger *>(e)),
                tag_elements[elt_idx].name);
      break;

    case ebmlt_string:
      o->printf("%s</%s>\n", string(*dynamic_cast<EbmlString *>(e)).c_str(),
                tag_elements[elt_idx].name);
      break;

    case ebmlt_ustring:
      s = UTFstring_to_cstrutf8(UTFstring(*static_cast
                                          <EbmlUnicodeString *>(e)).c_str());
      o->printf("%s</%s>\n", s, tag_elements[elt_idx].name);
      safefree(s);
      break;

    case ebmlt_time:
      o->printf(FMT_TIMECODEN "</%s>\n", 
                ARG_TIMECODEN(uint64(*dynamic_cast<EbmlUInteger *>(e))),
                tag_elements[elt_idx].name);
      break;

    case ebmlt_binary:
      o->printf("FIXME :(</%s>\n", tag_elements[elt_idx].name);
      assert(false);
      break;

    default:
      assert(false);
  }
}

static int
tet_index(const char *name) {
  int i;

  for (i = 0; tag_elements[i].name != NULL; i++)
    if (!strcmp(name, tag_elements[i].name))
      return i;

  assert(false);
  return -1;
}

void
write_tags_xml(KaxTags &tags,
               mm_io_c *out) {
  int i;

  for (i = 0; tag_elements[i].name != NULL; i++) {
    tag_elements[i].start_hook = NULL;
    tag_elements[i].end_hook = NULL;
  }

  o = out;

  for (i = 0; i < tags.ListSize(); i++)
    write_xml_element_rec(1, 0, tags[i]);
}
