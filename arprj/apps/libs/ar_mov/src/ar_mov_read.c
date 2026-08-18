
#include "ar_mov_def.h"
#include "ar_mov_utility.h"
#include "ar_mov_api.h"

AR_U8 big_to_little_get_u8(void *src)
{
    AR_U8 *p = src;
    return *p;
}

AR_U16 big_to_little_get_u16(void *src)
{
    AR_U8 *p = src;
    AR_U16 tmp = (AR_U16)((p[1])|(p[0])<<8);
    return tmp;
}

void big_to_little_get_u24(void *src, void *dst)
{
    AR_U8 *p = src;
    AR_U8 *q = dst;
    *(q + 2) = *(p + 0);
    *(q + 1) = *(p + 1);
    *(q + 0) = *(p + 2);
}


AR_U32 big_to_little_get_u32(void *src)
{
    AR_U8 *p = src;
    AR_U32 tmp = (AR_U32)((p[3])|(p[2])<<8|(p[1])<<16|(p[0])<<24);
    return tmp;
}


int mov_u32t_to_time(int time1)
{
    struct tm *p;
    struct tm tt = {0};

    time_t time_19040101 = -2082873957;
    time_t timep = time_19040101 + time1;

    p = localtime_r(&timep, &tt);
    //p = gmtime_r(&timep, &tt);

    printf("time: %04d%02d%02d%02d%02d%02d\n", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
                                       p->tm_hour, p->tm_min, p->tm_sec);

    return 0;
}

//#define MOV_DEBUG 1
#if 1
static void mov_print_ftyp_atom(MovFtypAtom *atom)
{
#ifdef MOV_DEBUG
    printf("mov ftyp atom info:\n");
    printf("size: 0x%x\n", atom->ftyp_size);
    char *tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("major_brand:0x%x\n", atom->major_brand);
    printf("minor_version:0x%x\n", atom->minor_version);
    printf("compatible_brands:0x%x\n", atom->compatible_brands);
    printf("\n\n");
#endif	
}

static void mov_print_atom_header(MovAtomHeader *atom, char *name)
{
#ifdef MOV_DEBUG
    char *tmp = NULL;
    printf("mov %s atom info:\n", name);
    printf("size:0x%x\n", atom->size);
  
    tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("\n\n");
#endif	
}

static void mov_print_mvhd_atom(MovMvhdAtom *atom)
{
#ifdef MOV_DEBUG
    printf("mov mvhd atom info:\n");
    printf("size:%u 0x%x\n", atom->mvhd_size, atom->mvhd_size);
    char *tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("creation_time:%d\n", atom->creation_time);
    mov_u32t_to_time(atom->creation_time);
    printf("modification_time:%d\n", atom->modification_time);
    mov_u32t_to_time(atom->modification_time);

    printf("time_scale:%u\n", atom->time_scale);
    printf("duration:%u\n", atom->duration);
    printf("rate:0x%08x\n", atom->rate);
    printf("volume:0x%04x\n", atom->volume);

    printf("matrix_struct: ");
    int i = 0;
    for(i = 0; i < 36; i ++)
    {
        printf("%u ", atom->matrix_struct[i]);
    }
    printf("\n");

    printf("preview_time:%u\n", atom->preview_time);
    printf("preview_duration:%u\n", atom->preview_duration);
    printf("poster_time:%u\n", atom->poster_time);
    printf("select_time:%u\n", atom->select_time);
    printf("select_duration:%u\n", atom->select_duration);
    printf("current_time:%u\n", atom->current_time);
    printf("next_track_id:%u\n", atom->next_track_id);

    printf("\n\n");
#endif	
}

static void mov_print_tkhd_atom(MovTkhdAtom *atom)
{
#ifdef MOV_DEBUG
    printf("mov tkhd atom info:\n");
    printf("size:%u 0x%x\n", atom->tkhd_size, atom->tkhd_size);
    char *tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);
    printf("creation_time:%d\n", atom->creation_time);
    mov_u32t_to_time(atom->creation_time);
    printf("modification_time:%d\n", atom->modification_time);
    mov_u32t_to_time(atom->modification_time);

    printf("track_id:%u\n", atom->track_id);
    printf("duration:%u\n", atom->duration);
    printf("layer:%u\n", atom->layer);
    printf("alternate_group:%u\n", atom->alternate_group);
    printf("volume:%u\n", atom->volume);

    printf("matrix_struct: ");
    int i = 0;
    for(i = 0; i < 36; i ++)
    {
        printf("%u ", atom->matrix_struct[i]);
    }
    printf("\n");

    printf("track_width:0x%08x\n", atom->track_width);
    printf("track_height:0x%08x\n", atom->track_height);

    printf("\n\n");
#endif	
}

static void mov_print_mdhd_atom(MovMdhdAtom *atom)
{
#ifdef MOV_DEBUG
    printf("mov mdhd atom info:\n");
    printf("size:%u 0x%x\n", atom->mdhd_size, atom->mdhd_size);
    char *tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);
    printf("creation_time:%d\n", atom->creation_time);
    mov_u32t_to_time(atom->creation_time);
    printf("modification_time:%d\n", atom->modification_time);
    mov_u32t_to_time(atom->modification_time);

    printf("time_scale:%u\n", atom->time_scale);
    printf("duration:%u\n", atom->duration);
    printf("language:%u\n", atom->language);
    printf("quality:%u\n", atom->quality);

    printf("\n\n");
#endif	
}

static void mov_print_hdlr_atom(MovHdlrAtom *atom, char *name)
{
#ifdef MOV_DEBUG

    printf("mov hdlr %s atom info:\n", name);
    printf("size:%u 0x%x\n", atom->hdlr_size, atom->hdlr_size);
    char *tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    tmp = (char *)&(atom->component_type);
    printf("component_type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    tmp = (char *)&(atom->component_subtype);
    printf("component_subtype:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("component_manufacturer:%u\n", atom->component_manufacturer);
    printf("component_flags:%u\n", atom->component_flags);
    printf("component_flags_mask:%u\n", atom->component_flags_mask);
#endif	
}


static void mov_print_vmhd_atom(MovVmhdAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov vmhd atom info:\n");
    printf("size:%u 0x%x\n", atom->vmhd_size, atom->vmhd_size);
    char *tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:%u\n", atom->v_f_s.flags);

    printf("graphics_mode:%u\n", atom->graphics_mode);
    printf("opcolor_red:%u\n", atom->opcolor_red);
    printf("opcolor_green:%u\n", atom->opcolor_green);
    printf("opcolor_blue:%u\n", atom->opcolor_blue);

    printf("\n\n");
#endif	
}

static void mov_print_smhd_atom(MovSmhdAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov smhd atom info:\n");
    printf("size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:%u\n", atom->v_f_s.flags);

    printf("balance:%u\n", atom->balance);
    printf("\n\n");
#endif	
}

static void mov_print_dinf_atom(MovDinfAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov dinf atom info:\n");
    printf("dinf size:%u 0x%x\n", atom->dinf_size, atom->dinf_size);
    char *tmp = (char *)&(atom->type);
    printf("dinf type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("dref size:%u 0x%x\n", atom->dref_atom.dref_size, atom->dref_atom.dref_size);
    tmp = (char *)&(atom->dref_atom.type);
    printf("dref type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("dref version:%u\n", atom->dref_atom.v_f_s.version);
    printf("dref flags:%u\n", atom->dref_atom.v_f_s.flags);
    printf("dref entry_count:%u\n", atom->dref_atom.entry_count);

    printf("url size:%u 0x%x\n", atom->dref_atom.url_atom.url_size, atom->dref_atom.url_atom.url_size);
    tmp = (char *)&(atom->dref_atom.url_atom.type);
    printf("url type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("url version:%u\n", atom->dref_atom.url_atom.v_f_s.version);
    printf("url flags:%u\n", atom->dref_atom.url_atom.v_f_s.flags);

    printf("\n\n");
#endif	
}


static void mov_print_stbl(MovPBHandle *pPBHandle)
{
#ifdef MOV_DEBUG

    printf("mov stbl info:\n");
    printf("stsd -- offset:%u size:%u 0x%x\n", pPBHandle->pMovHead.stsd_offset, pPBHandle->pMovHead.stsd_size, pPBHandle->pMovHead.stsd_size);
    printf("stts -- offset:%u size:%u 0x%x\n", pPBHandle->pMovHead.stts_offset, pPBHandle->pMovHead.stts_size, pPBHandle->pMovHead.stts_size);
    printf("stss -- offset:%u size:%u 0x%x\n", pPBHandle->pMovHead.stss_offset, pPBHandle->pMovHead.stss_size, pPBHandle->pMovHead.stss_size);
    printf("stsc -- offset:%u size:%u 0x%x\n", pPBHandle->pMovHead.stsc_offset, pPBHandle->pMovHead.stsc_size, pPBHandle->pMovHead.stsc_size);
    printf("stsz -- offset:%u size:%u 0x%x\n", pPBHandle->pMovHead.stsz_offset, pPBHandle->pMovHead.stsz_size, pPBHandle->pMovHead.stsz_size);
    printf("stco -- offset:%u size:%u 0x%x\n", pPBHandle->pMovHead.stco_offset, pPBHandle->pMovHead.stco_size, pPBHandle->pMovHead.stco_size);

    printf("\n\n");
#endif	
}

static void mov_print_udta(void *buf, int len)
{
#ifdef MOV_DEBUG

    AR_U8 *p = buf;
    int i = 0;
    printf("mov udta info:\n");
    for (i = 0; i < len; i ++)
    {
        printf("%02x ", p[i]);
        if ( (i + 1) % 10 == 0)
            printf("\n");
    }
    printf("\n\n");
#endif	
}

static void mov_print_stsd(MovStsdAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov stsd atom info:\n");
    printf("stsd size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("stsd type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("counts:%u\n", atom->counts);
    int i, j;
    printf("table info:\n");
    for (i = 0; i < atom->counts; i ++)
    {
        printf("table[%d]:\n", i);
        printf("description_size:%u\n", atom->tables[i].description_size);
        tmp = (char *)&(atom->tables[i].data_format);
        printf("data_format:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
        printf("data_reference_index:%u\n", atom->tables[i].data_reference_index);
        printf("version:%u\n", atom->tables[i].version);
        printf("revision_level:%u\n", atom->tables[i].revision_level);
        tmp = (char *)&(atom->tables[i].vendor);
        printf("vendor:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
        printf("temporal_qulity:%u\n", atom->tables[i].temporal_qulity);
        printf("spatial_qulity:%u\n", atom->tables[i].spatial_qulity);
        printf("width:%u\n", atom->tables[i].width);
        printf("height:%u\n", atom->tables[i].height);
        printf("h_resolution:%u\n", atom->tables[i].h_resolution);
        printf("v_resolution:%u\n", atom->tables[i].v_resolution);
        printf("data_size:%u\n", atom->tables[i].data_size);
        printf("frame_count:%u\n", atom->tables[i].frame_count);
    }

    printf("\n\n");
#endif	
}

static void mov_print_stsd_soun(MovStsdAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov stsd atom info:\n");
    printf("stsd size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("stsd type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("counts:%u\n", atom->counts);
    int i, j;
    printf("table info:\n");

    if (atom->tables)
    {
        for (i = 0; i < atom->counts; i ++)
        {
            printf("table[%d]:\n", i);
            printf("description_size:%u\n", atom->tables[i].description_size);
            tmp = (char *)&(atom->tables[i].data_format);
            printf("data_format:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
            printf("data_reference_index:%u\n", atom->tables[i].data_reference_index);
            printf("version:%u\n", atom->tables[i].version);
            printf("revision_level:%u\n", atom->tables[i].revision_level);
            tmp = (char *)&(atom->tables[i].vendor);
            printf("vendor:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
            printf("temporal_qulity:%u\n", atom->tables[i].temporal_qulity);
            printf("spatial_qulity:%u\n", atom->tables[i].spatial_qulity);
            printf("width:%u\n", atom->tables[i].width);
            printf("height:%u\n", atom->tables[i].height);
            printf("h_resolution:%u\n", atom->tables[i].h_resolution);
            printf("v_resolution:%u\n", atom->tables[i].v_resolution);
            printf("data_size:%u\n", atom->tables[i].data_size);
            printf("frame_count:%u\n", atom->tables[i].frame_count);

            printf("stsd data info:\n");
            int len = atom->tables[i].description_size - 76;
            for (j = 0; j < len; j ++)
            {
                printf("%02x ", atom->tables[i].data[j]);
                if ( (j + 1) % 16 == 0)
                    printf("\n");
            }

        }

    }

    if (atom->s_tables)
    {
        for (i = 0; i < atom->counts; i ++)
        {
            printf("s_tables[%d]:\n", i);
            printf("description_size:%u\n", atom->s_tables[i].description_size);
            tmp = (char *)&(atom->s_tables[i].data_format);
            printf("data_format:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
            printf("data_reference_index:%u\n", atom->s_tables[i].data_reference_index);
            printf("version:%u\n", atom->s_tables[i].version);
            printf("revision_level:%u\n", atom->s_tables[i].revision_level);
            tmp = (char *)&(atom->s_tables[i].vendor);
            printf("vendor:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);

            printf("number_of_channels:%u\n", atom->s_tables[i].number_of_channels);
            printf("sample_size:%u\n", atom->s_tables[i].sample_size);
            printf("compression_id:%u\n", atom->s_tables[i].compression_id);
            printf("packet_size:%u\n", atom->s_tables[i].packet_size);
            printf("sample_rate:%u\n", atom->s_tables[i].sample_rate);
        }
    }

    printf("\n\n");
#endif	
}


static void mov_print_stts(MovSttsAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov stts atom info:\n");
    printf("stts size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("stts type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("counts:%u\n", atom->counts);
    int i;
    printf("table info:\n");
    for (i = 0; i < atom->counts; i ++)
    {
        printf("%d - count:%u\n", i, atom->tables[i].count);
        printf("%d - duration:%u\n", i, atom->tables[i].duration);
    }

    printf("\n\n");
#endif	
}

static void mov_print_stss(MovStssAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov stss atom info:\n");
    printf("stss size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("stss type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("counts:%u\n", atom->counts);
    int i;
    printf("table info:\n");
    for (i = 0; i < atom->counts; i ++)
    {
        printf("%d - number:%u\n", i, atom->tables[i].number);
    }

    printf("\n\n");
#endif	
}

static void mov_print_stsc(MovStscAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov stsc atom info:\n");
    printf("stsc size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("stsc type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("counts:%u\n", atom->counts);
    int i;
    printf("table info:\n");
    for (i = 0; i < atom->counts; i ++)
    {
        printf("%d - first_chunk:%u\n", i, atom->tables[i].first_chunk);
        printf("%d - samples_per_chunk:%u\n", i, atom->tables[i].samples_per_chunk);
        printf("%d - samples_description_id:%u\n", i, atom->tables[i].samples_description_id);
    }

    printf("\n\n");
#endif	
}

static void mov_print_stsz(MovStszAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov stsz atom info:\n");
    printf("stsz size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("stsz type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("sample_size:%u\n", atom->sample_size);
    printf("counts:%u\n", atom->counts);
    int i;
    printf("table info:\n");
    if (atom->tables)
    {
        for (i = 0; i < atom->counts; i ++)
        {
            printf("%d - size:%u\n", i, atom->tables[i].size);
        }
    }

    printf("\n\n");
#endif	
}

static void mov_print_stco(MovStcoAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov stco atom info:\n");
    printf("stco size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("stco type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("counts:%u\n", atom->counts);
    int i;
    printf("table info:\n");
    for (i = 0; i < atom->counts; i ++)
    {
        printf("%d - offset:%u\n", i, atom->tables[i].offset);
    }

    printf("\n\n");
#endif	
}

static void mov_print_elst_atom(MovElstAtom *atom)
{
#ifdef MOV_DEBUG

    printf("mov elst atom info:\n");
    printf("elst size:%u 0x%x\n", atom->size, atom->size);
    char *tmp = (char *)&(atom->type);
    printf("elst type:%c %c %c %c\n", tmp[3], tmp[2], tmp[1], tmp[0]);
    printf("type:0x%02x %02x %02x %02x\n", tmp[3], tmp[2], tmp[1], tmp[0]);

    printf("version:%u\n", atom->v_f_s.version);
    printf("flags:0x%x\n", atom->v_f_s.flags);

    printf("counts:%u\n", atom->counts);
    int i;
    printf("table info:\n");
    for (i = 0; i < atom->counts; i ++)
    {
        printf("%d - track_duration:%u\n", i, atom->tables[i].track_duration);
        printf("%d - media_time:%u\n", i, atom->tables[i].media_time);
        printf("%d - media_rate:%u\n", i, atom->tables[i].media_rate);
    }

    printf("\n\n");
#endif	
}


int check_stbl_type(AR_U32 type, AR_U32 offset, AR_U32 size, MovPBHandle *pPBHandle)
{
    switch (type) {
        case MAKE_FCC('s','t','s','d'):
            pPBHandle->pMovHead.stsd_size = size;
            pPBHandle->pMovHead.stsd_offset = offset;
            break;

        case MAKE_FCC('s','t','t','s'):
            pPBHandle->pMovHead.stts_size = size;
            pPBHandle->pMovHead.stts_offset = offset;
            break;

        case MAKE_FCC('s','t','s','s'):
            pPBHandle->pMovHead.stss_size = size;
            pPBHandle->pMovHead.stss_offset = offset;
            break;

        case MAKE_FCC('s','t','s','c'):
            pPBHandle->pMovHead.stsc_size = size;
            pPBHandle->pMovHead.stsc_offset = offset;
            break;

        case MAKE_FCC('s','t','s','z'):
            pPBHandle->pMovHead.stsz_size = size;
            pPBHandle->pMovHead.stsz_offset = offset;
            break;

        case MAKE_FCC('s','t','c','o'):
            pPBHandle->pMovHead.stco_size = size;
            pPBHandle->pMovHead.stco_offset = offset;
            break;

        default:
            printf("error type:0x%02x\n", type);
            break;
    }

    return 0;
}

int big_to_little_endian(void *src, void *dst, int size)
{
    char *p = src;
    unsigned int *tmp = (unsigned int *)dst;
    int count = (size + 3) / 4;
    int i = 0;
    for (i = 0; i < count; i ++)
    {
        *tmp = (unsigned int)((p[3])|(p[2])<<8|(p[1])<<16|(p[0])<<24);
        tmp ++;
        p += 4;
    }
    return 0;
}
#endif

static int mov_read_header(MovPBHandle *pPBHandle)
{
    int ret = 0;
    int i = 0;
    
    if(!pPBHandle)
    {
        printf("[%s][%d] pBHandle is NULL \n", __func__, __LINE__);
        return -1;
    }

	char * mov_buf = NULL;
	fseek(pPBHandle->file, 0, SEEK_END);
	int file_size = ftell(pPBHandle->file);
    mov_buf = (char *)malloc(file_size);
	if(!mov_buf)
	{
		printf("[%s][%d]mov buf malloc failed \n", __func__, __LINE__);
		return -1;
	}
	
	fseek(pPBHandle->file, 0, SEEK_SET);
    ret = fread(mov_buf, 1, file_size, pPBHandle->file);
    if(ret != file_size)
    {
        printf("[%s][%d]read failed. ret = %d\n", __func__, __LINE__, ret);
        goto Exit;
    }

	int offset = 0;
    /* ftyp */
    MovFtypAtom ftyp_atom = {0};
    ftyp_atom.ftyp_size = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    ftyp_atom.type = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    ftyp_atom.major_brand = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    ftyp_atom.minor_version = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    ftyp_atom.compatible_brands = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    mov_print_ftyp_atom(&ftyp_atom);
    pPBHandle->pMovHead.ftyp_size = ftyp_atom.ftyp_size;

    /* wide */
    MovWideAtom wide_atom = {0};
    wide_atom.size = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    wide_atom.type = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    mov_print_atom_header(&wide_atom, "wide");
    pPBHandle->pMovHead.wide_size = wide_atom.size;
    pPBHandle->pMovHead.mdat_offset = sizeof(MovFtypAtom) + sizeof(MovWideAtom);

    /* mdat */
    MovMdatAtom mdat_atom = {0};
    mdat_atom.size = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    mdat_atom.type = big_to_little_get_u32(mov_buf + offset);
    offset += 4;
    mov_print_atom_header(&mdat_atom, "mdat");
    pPBHandle->pMovHead.mdat_size = mdat_atom.size;
    pPBHandle->pMovHead.moov_offset = mdat_atom.size + pPBHandle->pMovHead.mdat_offset;
	
	char * buf = mov_buf + pPBHandle->pMovHead.moov_offset;
	offset = 0;
	/* moov */
    MovMoovAtom moov_atom = {0};
    moov_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    moov_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_atom_header(&moov_atom, "moov");
    pPBHandle->pMovHead.moov_size = moov_atom.size;

    /* mvhd */
    MovMvhdAtom mvhd_atom = {0};
    mvhd_atom.mvhd_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, mvhd_atom.v_f_c + 1);
    offset += 3;
    mvhd_atom.creation_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.modification_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.time_scale = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.duration = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.rate = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.volume = big_to_little_get_u16(buf + offset);
    offset += 2;
    offset += 10;
    memcpy(mvhd_atom.matrix_struct, buf + offset, 36);
    offset += 36;
    mvhd_atom.preview_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.preview_duration = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.poster_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.select_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.select_duration = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.current_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mvhd_atom.next_track_id = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_mvhd_atom(&mvhd_atom);

    /* trak */
    MovTrakAtom trak_atom = {0};
    trak_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    trak_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_atom_header(&trak_atom, "trak");
	
    /* tkhd */
    MovTkhdAtom tkhd_atom = {0};
    tkhd_atom.tkhd_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    tkhd_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    tkhd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, tkhd_atom.v_f_c + 1);
    offset += 3;
    tkhd_atom.creation_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    tkhd_atom.modification_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    tkhd_atom.track_id = big_to_little_get_u32(buf + offset);
    offset += 4;
    offset += 4; //res1
    tkhd_atom.duration = big_to_little_get_u32(buf + offset);
    offset += 4;
    offset += 8; //res2
    tkhd_atom.layer = big_to_little_get_u16(buf + offset);
    offset += 2;
    tkhd_atom.alternate_group = big_to_little_get_u16(buf + offset);
    offset += 2;
    tkhd_atom.volume = big_to_little_get_u16(buf + offset);
    offset += 2;
    offset += 2; //res3
    memcpy(tkhd_atom.matrix_struct, buf + offset, 36);
    offset += 36;
    tkhd_atom.track_width = big_to_little_get_u32(buf + offset);
    offset += 4;
    tkhd_atom.track_height = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_tkhd_atom(&tkhd_atom);

	pPBHandle->video_width = tkhd_atom.track_width>>16;
	pPBHandle->video_height = tkhd_atom.track_height>>16;

    /* edts */
    MovEdtsAtom edts_atom = {0};
    edts_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    edts_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_atom_header(&edts_atom, "edts");

    /* elst */
    MovElstAtom elst_atom = {0};
    elst_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    elst_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    elst_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, elst_atom.v_f_c + 1);
    offset += 3;

    elst_atom.counts = big_to_little_get_u32(buf + offset);
    offset += 4;
    elst_atom.tables = (MovElstSample *)malloc(elst_atom.counts * sizeof(MovElstSample));
    if (elst_atom.tables == NULL)
    {
        printf("[%s][%d]malloc failed \n", __func__, __LINE__);
        goto Exit;
    }

    for (i = 0; i < elst_atom.counts; i ++)
    {
        elst_atom.tables[i].track_duration = big_to_little_get_u32(buf + offset);
        offset += 4;
        elst_atom.tables[i].media_time = big_to_little_get_u32(buf + offset);
        offset += 4;
        elst_atom.tables[i].media_rate = big_to_little_get_u32(buf + offset);
        offset += 4;
    }
    mov_print_elst_atom(&elst_atom);
	free(elst_atom.tables);

    /* mdia */
    MovMdiaAtom mdia_atom = {0};
    mdia_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    mdia_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_atom_header(&mdia_atom, "mdia");

    /* mdhd */
    MovMdhdAtom mdhd_atom = {0};
    mdhd_atom.mdhd_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    mdhd_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mdhd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, mdhd_atom.v_f_c + 1);
    offset += 3;
    mdhd_atom.creation_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mdhd_atom.modification_time = big_to_little_get_u32(buf + offset);
    offset += 4;
    mdhd_atom.time_scale = big_to_little_get_u32(buf + offset);
    offset += 4;
    mdhd_atom.duration = big_to_little_get_u32(buf + offset);
    offset += 4;
    mdhd_atom.language = big_to_little_get_u16(buf + offset);
    offset += 2;
    mdhd_atom.quality = big_to_little_get_u16(buf + offset);
    offset += 2;
    mov_print_mdhd_atom(&mdhd_atom);

    /* hdlr mhlr */
    MovHdlrAtom hdlr_mhlr_atom = {0};
    hdlr_mhlr_atom.hdlr_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_mhlr_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_mhlr_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, hdlr_mhlr_atom.v_f_c + 1);
    offset += 3;
    hdlr_mhlr_atom.component_type = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_mhlr_atom.component_subtype = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_mhlr_atom.component_manufacturer = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_mhlr_atom.component_flags = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_mhlr_atom.component_flags_mask= big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_hdlr_atom(&hdlr_mhlr_atom, "mhlr");

    /* hdlr mhlr name */
    offset += 1;
    AR_U32 hdlr_mhlr_name_len = hdlr_mhlr_atom.hdlr_size - sizeof(MovHdlrAtom);
	offset += hdlr_mhlr_name_len - 1;

    /* minf */
    MovMinfAtom minf_atom = {0};
    minf_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    minf_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_atom_header(&minf_atom, "minf");
    pPBHandle->pMovHead.minf_size = minf_atom.size;

    /* vmhd */
    MovVmhdAtom vmhd_atom = {0};
    vmhd_atom.vmhd_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    vmhd_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    vmhd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, vmhd_atom.v_f_c + 1);
    offset += 3;
    vmhd_atom.graphics_mode = big_to_little_get_u16(buf + offset);
    offset += 2;
    vmhd_atom.opcolor_red = big_to_little_get_u16(buf + offset);
    offset += 2;
    vmhd_atom.opcolor_green = big_to_little_get_u16(buf + offset);
    offset += 2;
    vmhd_atom.opcolor_blue = big_to_little_get_u16(buf + offset);
    offset += 2;
    mov_print_vmhd_atom(&vmhd_atom);

    /* hdlr dhlr */
    MovHdlrAtom hdlr_dhlr_atom = {0};
    hdlr_dhlr_atom.hdlr_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_dhlr_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_dhlr_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, hdlr_dhlr_atom.v_f_c + 1);
    offset += 3;
    hdlr_dhlr_atom.component_type = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_dhlr_atom.component_subtype = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_dhlr_atom.component_manufacturer = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_dhlr_atom.component_flags = big_to_little_get_u32(buf + offset);
    offset += 4;
    hdlr_dhlr_atom.component_flags_mask= big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_hdlr_atom(&hdlr_dhlr_atom, "dhlr");

    /* hdlr dhlr name */
    offset += 1;
    AR_U32 hdlr_dhlr_name_len = hdlr_dhlr_atom.hdlr_size - sizeof(MovHdlrAtom);
    offset += hdlr_dhlr_name_len - 1;

    /* dinf */
    MovDinfAtom dinf_atom = {0};
    dinf_atom.dinf_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    dinf_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    /* dinf dref */
    dinf_atom.dref_atom.dref_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    dinf_atom.dref_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    dinf_atom.dref_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, dinf_atom.dref_atom.v_f_c + 1);
    offset += 3;
    dinf_atom.dref_atom.entry_count = big_to_little_get_u32(buf + offset);
    offset += 4;
    /* dinf dref url */
    dinf_atom.dref_atom.url_atom.url_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    dinf_atom.dref_atom.url_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    dinf_atom.dref_atom.url_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, dinf_atom.dref_atom.url_atom.v_f_c + 1);
    offset += 3;
    mov_print_dinf_atom(&dinf_atom);

    /* stbl */
    pPBHandle->pMovHead.stbl_offset = offset;
    MovStblAtom stbl_atom = {0};
    stbl_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stbl_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_atom_header(&stbl_atom, "stbl");

    /* stsd */
    MovStsdAtom stsd_atom = {0};
    stsd_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stsd_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    check_stbl_type(stsd_atom.type, offset - 8, stsd_atom.size, pPBHandle);
    stsd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, stsd_atom.v_f_c + 1);
    offset += 3;
    stsd_atom.counts = big_to_little_get_u32(buf + offset);
    offset += 4;
    stsd_atom.tables = (MovStsdSample *)malloc(stsd_atom.counts * sizeof(MovStsdSample));
    if (stsd_atom.tables == NULL)
    {
        printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
        goto Exit;
    }

    for (i = 0; i < stsd_atom.counts; i++)
    {
        stsd_atom.tables[i].description_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.tables[i].data_format = big_to_little_get_u32(buf + offset);
        offset += 4;
        offset += 6; //res
        stsd_atom.tables[i].data_reference_index = big_to_little_get_u16(buf + offset);
        offset += 2;
        stsd_atom.tables[i].version = big_to_little_get_u16(buf + offset);
        offset += 2;
        stsd_atom.tables[i].revision_level = big_to_little_get_u16(buf + offset);
        offset += 2;
        stsd_atom.tables[i].vendor = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.tables[i].temporal_qulity = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.tables[i].spatial_qulity = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.tables[i].width = big_to_little_get_u16(buf + offset);
        offset += 2;
        stsd_atom.tables[i].height = big_to_little_get_u16(buf + offset);
        offset += 2;
        stsd_atom.tables[i].h_resolution = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.tables[i].v_resolution = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.tables[i].data_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.tables[i].frame_count = big_to_little_get_u16(buf + offset);
        offset += 2;
        offset += 26; //res1, res2

        int leave_len = stsd_atom.tables[i].description_size - 76;
        offset += leave_len;
    }
    mov_print_stsd(&stsd_atom);	
	pPBHandle->v_enc_type = (MAKE_FCC('h','v','c','C') == stsd_atom.tables->data_format)?1:0;//0:H265 1:H264
	free(stsd_atom.tables);

    /* stts */
    MovSttsAtom stts_atom = {0};
    stts_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stts_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    check_stbl_type(stts_atom.type, offset - 8, stts_atom.size, pPBHandle);
    stts_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, stts_atom.v_f_c + 1);
    offset += 3;
    stts_atom.counts = big_to_little_get_u32(buf + offset);
    offset += 4;
    stts_atom.tables = (MovSttsSample *)malloc(stts_atom.counts * sizeof(MovSttsSample));
    if (stts_atom.tables == NULL)
    {
        printf("[%s][%d]malloc failed.\n", __func__,__LINE__);
        goto Exit;
    }

    for (i = 0; i < stts_atom.counts; i ++)
    {
        stts_atom.tables[i].count = big_to_little_get_u32(buf + offset);
        offset += 4;
        stts_atom.tables[i].duration = big_to_little_get_u32(buf + offset);
        offset += 4;
    }
    mov_print_stts(&stts_atom);
	free(stts_atom.tables);

    /* stss */
    MovStssAtom stss_atom = {0};
    stss_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stss_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    check_stbl_type(stss_atom.type, offset - 8, stss_atom.size, pPBHandle);
    stss_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, stss_atom.v_f_c + 1);
    offset += 3;
    stss_atom.counts = big_to_little_get_u32(buf + offset);
    offset += 4;
    stss_atom.tables = (MovStssSample *)malloc(stss_atom.counts * sizeof(MovStssSample));
    if (stss_atom.tables == NULL)
    {
        printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
        goto Exit;
    }

    for (i = 0; i < stss_atom.counts; i ++)
    {
        stss_atom.tables[i].number = big_to_little_get_u32(buf + offset);
        offset += 4;
    }
    mov_print_stss(&stss_atom);
	free(stss_atom.tables);

    /* stsc */
    MovStscAtom stsc_atom = {0};
    stsc_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stsc_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    check_stbl_type(stsc_atom.type, offset - 8, stsc_atom.size, pPBHandle);
    stsc_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, stsc_atom.v_f_c + 1);
    offset += 3;
    stsc_atom.counts = big_to_little_get_u32(buf + offset);
    offset += 4;
    stsc_atom.tables = (MovStscSample *)malloc(stsc_atom.counts * sizeof(MovStscSample));
    if (stsc_atom.tables == NULL)
    {
        printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
        goto Exit;
    }

    for (i = 0; i < stsc_atom.counts; i ++)
    {
        stsc_atom.tables[i].first_chunk = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsc_atom.tables[i].samples_per_chunk = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsc_atom.tables[i].samples_description_id = big_to_little_get_u32(buf + offset);
        offset += 4;
    }
    mov_print_stsc(&stsc_atom);
	free(stsc_atom.tables);

    /* stsz */
    MovStszAtom stsz_atom = {0};
    stsz_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stsz_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    check_stbl_type(stsz_atom.type, offset - 8, stsz_atom.size, pPBHandle);
    stsz_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, stsz_atom.v_f_c + 1);
    offset += 3;
    stsz_atom.sample_size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stsz_atom.counts = big_to_little_get_u32(buf + offset);
    offset += 4;
    stsz_atom.tables = (MovStszSample *)malloc(stsz_atom.counts * sizeof(MovStszSample));
    if (stsz_atom.tables == NULL)
    {
        printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
        goto Exit;
    }

    for (i = 0; i < stsz_atom.counts; i ++)
    {
        stsz_atom.tables[i].size = big_to_little_get_u32(buf + offset);
        offset += 4;
    }
    mov_print_stsz(&stsz_atom);

    /* stco */
    MovStcoAtom stco_atom = {0};
    stco_atom.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    stco_atom.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    check_stbl_type(stco_atom.type, offset - 8, stco_atom.size, pPBHandle);
    stco_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
    offset += 1;
    big_to_little_get_u24(buf + offset, stco_atom.v_f_c + 1);
    offset += 3;
    stco_atom.counts = big_to_little_get_u32(buf + offset);
    offset += 4;
    stco_atom.tables = (MovStcoSample *)malloc(stco_atom.counts * sizeof(MovStcoSample));
    if (stco_atom.tables == NULL)
    {
        printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
		free(stsz_atom.tables);
        goto Exit;
    }

    for (i = 0; i < stco_atom.counts; i ++)
    {
        stco_atom.tables[i].offset = big_to_little_get_u32(buf + offset);
        offset += 4;
    }
    mov_print_stco(&stco_atom);
    mov_print_stbl(pPBHandle);

	AR_S32 s32VideoCnt = stsz_atom.counts;
	pPBHandle->fps = mdhd_atom.time_scale/(mdhd_atom.duration/s32VideoCnt);
	pPBHandle->gop = round((s32VideoCnt - 1)/stss_atom.counts); 
	pPBHandle->idx_array_count = stco_atom.counts;
	
	pPBHandle->idx_array = (FrameInfo *)malloc(pPBHandle->idx_array_count * sizeof(FrameInfo));
	if (pPBHandle->idx_array == NULL)
    {
        printf("[%s][%d]pPBHandle->idx_array malloc failed.\n", __func__, __LINE__);
		free(stsz_atom.tables);
		free(stco_atom.tables);
        goto Exit;
    }

	for(i = 0; i < pPBHandle->idx_array_count; i++)
	{
		pPBHandle->idx_array[i].size = stsz_atom.tables[i].size;
		pPBHandle->idx_array[i].offset = stco_atom.tables[i].offset;
		pPBHandle->idx_array[i].no++;
	}

	if(stsz_atom.tables)
	{
        free(stsz_atom.tables);
		stsz_atom.tables = NULL;
	}
	if(stco_atom.tables)
	{
        free(stco_atom.tables);
		stco_atom.tables = NULL;
	}

	/*================================audio=======================================*/
    // audio
    #if 1
    /* trak */
    //MovTrakAtom trak_atom = {0};
    if(mvhd_atom.next_track_id == 3)
	{
		pPBHandle->audio_enable = 1;
		
        memset(&trak_atom, 0, sizeof(MovTrakAtom));
        trak_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        trak_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_atom_header(&trak_atom, "trak");
    
        /* tkhd */
        //MovTkhdAtom tkhd_atom = {0};
        memset(&tkhd_atom, 0, sizeof(MovTkhdAtom));
        tkhd_atom.tkhd_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        tkhd_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        tkhd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, tkhd_atom.v_f_c + 1);
        offset += 3;
        tkhd_atom.creation_time = big_to_little_get_u32(buf + offset);
        offset += 4;
        tkhd_atom.modification_time = big_to_little_get_u32(buf + offset);
        offset += 4;
        tkhd_atom.track_id = big_to_little_get_u32(buf + offset);
        offset += 4;
        offset += 4; //res1
        tkhd_atom.duration = big_to_little_get_u32(buf + offset);
        offset += 4;
        offset += 8; //res2
        tkhd_atom.layer = big_to_little_get_u16(buf + offset);
        offset += 2;
        tkhd_atom.alternate_group = big_to_little_get_u16(buf + offset);
        offset += 2;
        tkhd_atom.volume = big_to_little_get_u16(buf + offset);
        offset += 2;
        offset += 2; //res3
        memcpy(tkhd_atom.matrix_struct, buf + offset, 36);
        offset += 36;
        tkhd_atom.track_width = big_to_little_get_u32(buf + offset);
        offset += 4;
        tkhd_atom.track_height = big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_tkhd_atom(&tkhd_atom);
    
        /* edts */
        //MovEdtsAtom edts_atom = {0};
        memset(&edts_atom, 0, sizeof(MovEdtsAtom));
        edts_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        edts_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_atom_header(&edts_atom, "edts");
    
        /* elst */
        //MovElstAtom elst_atom = {0};
        memset(&elst_atom, 0, sizeof(MovElstAtom));
        elst_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        elst_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        elst_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, elst_atom.v_f_c + 1);
        offset += 3;
    
        elst_atom.counts = big_to_little_get_u32(buf + offset);
        offset += 4;
        elst_atom.tables = (MovElstSample *)malloc(elst_atom.counts * sizeof(MovElstSample));
        if (elst_atom.tables == NULL)
        {
            printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
            goto Exit;
        }
    
        for (i = 0; i < elst_atom.counts; i ++)
        {
            elst_atom.tables[i].track_duration = big_to_little_get_u32(buf + offset);
            offset += 4;
            elst_atom.tables[i].media_time = big_to_little_get_u32(buf + offset);
            offset += 4;
            elst_atom.tables[i].media_rate = big_to_little_get_u32(buf + offset);
            offset += 4;
        }
        mov_print_elst_atom(&elst_atom);
		free(elst_atom.tables);
    
    
        /* mdia */
        //MovMdiaAtom mdia_atom = {0};
        memset(&mdia_atom, 0, sizeof(MovMdiaAtom));
        mdia_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        mdia_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_atom_header(&mdia_atom, "mdia");
    
        /* mdhd */
        //MovMdhdAtom mdhd_atom = {0};
        memset(&mdhd_atom, 0, sizeof(MovMdhdAtom));
        mdhd_atom.mdhd_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        mdhd_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        mdhd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, mdhd_atom.v_f_c + 1);
        offset += 3;
        mdhd_atom.creation_time = big_to_little_get_u32(buf + offset);
        offset += 4;
        mdhd_atom.modification_time = big_to_little_get_u32(buf + offset);
        offset += 4;
        mdhd_atom.time_scale = big_to_little_get_u32(buf + offset);
        offset += 4;
        mdhd_atom.duration = big_to_little_get_u32(buf + offset);
        offset += 4;
        mdhd_atom.language = big_to_little_get_u16(buf + offset);
        offset += 2;
        mdhd_atom.quality = big_to_little_get_u16(buf + offset);
        offset += 2;
        mov_print_mdhd_atom(&mdhd_atom);
    
        /* hdlr mhlr */
        //MovHdlrAtom hdlr_mhlr_atom = {0};
        memset(&hdlr_mhlr_atom, 0, sizeof(MovHdlrAtom));
        hdlr_mhlr_atom.hdlr_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_mhlr_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_mhlr_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, hdlr_mhlr_atom.v_f_c + 1);
        offset += 3;
        hdlr_mhlr_atom.component_type = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_mhlr_atom.component_subtype = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_mhlr_atom.component_manufacturer = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_mhlr_atom.component_flags = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_mhlr_atom.component_flags_mask= big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_hdlr_atom(&hdlr_mhlr_atom, "mhlr");
    
        /* hdlr mhlr name */
        offset += 1;
        hdlr_mhlr_name_len = hdlr_mhlr_atom.hdlr_size - sizeof(MovHdlrAtom);  
        offset += hdlr_mhlr_name_len - 1;
    
        /* minf */
        //MovMinfAtom minf_atom = {0};
        memset(&minf_atom, 0, sizeof(MovMinfAtom));
        minf_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        minf_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_atom_header(&minf_atom, "minf");
        pPBHandle->pMovHead.minf_size = minf_atom.size;
    
        /* smhd */
        MovSmhdAtom smhd_atom = {0};
        smhd_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        smhd_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        smhd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, smhd_atom.v_f_c + 1);
        offset += 3;
        smhd_atom.balance = big_to_little_get_u16(buf + offset);
        offset += 2;
        offset += 2; //res
        mov_print_smhd_atom(&smhd_atom);
    
        /* hdlr dhlr */
        //MovHdlrAtom hdlr_dhlr_atom = {0};
        memset(&hdlr_dhlr_atom, 0, sizeof(MovHdlrAtom));
        hdlr_dhlr_atom.hdlr_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_dhlr_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_dhlr_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, hdlr_dhlr_atom.v_f_c + 1);
        offset += 3;
        hdlr_dhlr_atom.component_type = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_dhlr_atom.component_subtype = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_dhlr_atom.component_manufacturer = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_dhlr_atom.component_flags = big_to_little_get_u32(buf + offset);
        offset += 4;
        hdlr_dhlr_atom.component_flags_mask= big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_hdlr_atom(&hdlr_dhlr_atom, "dhlr");
    
        /* hdlr dhlr name */
        offset += 1;
        hdlr_dhlr_name_len = hdlr_dhlr_atom.hdlr_size - sizeof(MovHdlrAtom);
        offset += hdlr_dhlr_name_len - 1;
    
        /* dinf */
        //MovDinfAtom dinf_atom = {0};
        memset(&dinf_atom, 0, sizeof(MovDinfAtom));
        dinf_atom.dinf_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        dinf_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        /* dinf dref */
        dinf_atom.dref_atom.dref_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        dinf_atom.dref_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        dinf_atom.dref_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, dinf_atom.dref_atom.v_f_c + 1);
        offset += 3;
        dinf_atom.dref_atom.entry_count = big_to_little_get_u32(buf + offset);
        offset += 4;
        /* dinf dref url */
        dinf_atom.dref_atom.url_atom.url_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        dinf_atom.dref_atom.url_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        dinf_atom.dref_atom.url_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, dinf_atom.dref_atom.url_atom.v_f_c + 1);
        offset += 3;
        mov_print_dinf_atom(&dinf_atom);
    
        /* stbl */
        pPBHandle->pMovHead.stbl_offset = offset;
        //MovStblAtom stbl_atom = {0};
        memset(&stbl_atom, 0, sizeof(MovStblAtom));
        stbl_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stbl_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        mov_print_atom_header(&stbl_atom, "stbl");
    
    
        /* stsd */
        //MovStsdAtom stsd_atom = {0};
        memset(&stsd_atom, 0, sizeof(MovStsdAtom));
        stsd_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsd_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        //check_stbl_type(stsd_atom.type, offset - 8, stsd_atom.size, pPBHandle);
        stsd_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, stsd_atom.v_f_c + 1);
        offset += 3;
        stsd_atom.counts = big_to_little_get_u32(buf + offset);
        offset += 4;
    
        stsd_atom.s_tables = (MovStsdSounSample *)malloc(stsd_atom.counts * sizeof(MovStsdSounSample));
        if (stsd_atom.s_tables == NULL)
        {
            printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
            goto Exit;
        }
    
        for (i = 0; i < stsd_atom.counts; i ++)
        {
            stsd_atom.s_tables[i].description_size = big_to_little_get_u32(buf + offset);
            offset += 4;
            stsd_atom.s_tables[i].data_format = big_to_little_get_u32(buf + offset);
            offset += 4;
            offset += 6; //res
            stsd_atom.s_tables[i].data_reference_index = big_to_little_get_u16(buf + offset);
            offset += 2;
    
            stsd_atom.s_tables[i].version = big_to_little_get_u16(buf + offset);
            offset += 2;
            stsd_atom.s_tables[i].revision_level = big_to_little_get_u16(buf + offset);
            offset += 2;
            stsd_atom.s_tables[i].vendor = big_to_little_get_u32(buf + offset);
            offset += 4;
    
            stsd_atom.s_tables[i].number_of_channels = big_to_little_get_u16(buf + offset);
            offset += 2;
            stsd_atom.s_tables[i].sample_size = big_to_little_get_u16(buf + offset);
            offset += 2;
            stsd_atom.s_tables[i].compression_id = big_to_little_get_u16(buf + offset);
            offset += 2;
            stsd_atom.s_tables[i].packet_size = big_to_little_get_u16(buf + offset);
            offset += 2;
            stsd_atom.s_tables[i].sample_rate = big_to_little_get_u16(buf + offset);
            offset += 2;
            offset += 2;//res2
        }
		
		pPBHandle->a_rate = stsd_atom.s_tables->sample_rate;
		pPBHandle->a_channels = stsd_atom.s_tables->number_of_channels;
		pPBHandle->a_bits = stsd_atom.s_tables->sample_size;
        mov_print_stsd_soun(&stsd_atom);	
		free(stsd_atom.s_tables);

        AR_U32 tmp_size = big_to_little_get_u32(buf + offset);
        offset += tmp_size;
     
        /* stts */
        //MovSttsAtom stts_atom = {0};
        memset(&stts_atom, 0, sizeof(MovSttsAtom));
        stts_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stts_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        check_stbl_type(stts_atom.type, offset - 8, stts_atom.size, pPBHandle);
        stts_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, stts_atom.v_f_c + 1);
        offset += 3;
        stts_atom.counts = big_to_little_get_u32(buf + offset);
        offset += 4;
        stts_atom.tables = (MovSttsSample *)malloc(stts_atom.counts * sizeof(MovSttsSample));
        if (stts_atom.tables == NULL)
        {
            printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
            goto Exit;
        }
        for (i = 0; i < stts_atom.counts; i ++)
        {
            stts_atom.tables[i].count = big_to_little_get_u32(buf + offset);
            offset += 4;
            stts_atom.tables[i].duration = big_to_little_get_u32(buf + offset);
            offset += 4;
        }
        mov_print_stts(&stts_atom);
		free(stts_atom.tables);
    
        /* stsc */
        //MovStscAtom stsc_atom = {0};
        memset(&stsc_atom, 0, sizeof(MovStscAtom));
        stsc_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsc_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        check_stbl_type(stsc_atom.type, offset - 8, stsc_atom.size, pPBHandle);
        stsc_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, stsc_atom.v_f_c + 1);
        offset += 3;
        stsc_atom.counts = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsc_atom.tables = (MovStscSample *)malloc(stsc_atom.counts * sizeof(MovStscSample));
        if (stsc_atom.tables == NULL)
        {
            printf("[%s][%d]malloc failed.\n", __func__, __LINE__);
            goto Exit;
        }
    
        for (i = 0; i < stsc_atom.counts; i ++)
        {
            stsc_atom.tables[i].first_chunk = big_to_little_get_u32(buf + offset);
            offset += 4;
            stsc_atom.tables[i].samples_per_chunk = big_to_little_get_u32(buf + offset);
            offset += 4;
            stsc_atom.tables[i].samples_description_id = big_to_little_get_u32(buf + offset);
            offset += 4;
        }
        mov_print_stsc(&stsc_atom);  
		free(stsc_atom.tables);
    
        /* stsz */
        //MovStszAtom stsz_atom = {0};
        memset(&stsz_atom, 0, sizeof(MovStszAtom));
        stsz_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsz_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        check_stbl_type(stsz_atom.type, offset - 8, stsz_atom.size, pPBHandle);
        stsz_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, stsz_atom.v_f_c + 1);
        offset += 3;
        stsz_atom.sample_size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stsz_atom.counts = big_to_little_get_u32(buf + offset);
        offset += 4;
     
        stsz_atom.tables = (MovStszSample *)malloc(stsz_atom.counts * sizeof(MovStszSample));
        if (stsz_atom.tables == NULL)
        {
            printf("malloc failed.\n");
            goto Exit;

		}
        for (i = 0; i < stsz_atom.counts; i ++)
        {
            stsz_atom.tables[i].size = big_to_little_get_u32(buf + offset);
            offset += 4;
        }
     
        mov_print_stsz(&stsz_atom);
    
        /* stco */
        //MovStcoAtom stco_atom = {0};
        memset(&stco_atom, 0, sizeof(MovStcoAtom));
        stco_atom.size = big_to_little_get_u32(buf + offset);
        offset += 4;
        stco_atom.type = big_to_little_get_u32(buf + offset);
        offset += 4;
        check_stbl_type(stco_atom.type, offset - 8, stco_atom.size, pPBHandle);
        stco_atom.v_f_c[0] = big_to_little_get_u8(buf + offset);
        offset += 1;
        big_to_little_get_u24(buf + offset, stco_atom.v_f_c + 1);
        offset += 3;
        stco_atom.counts = big_to_little_get_u32(buf + offset);
        offset += 4;
        stco_atom.tables = (MovStcoSample *)malloc(stco_atom.counts * sizeof(MovStcoSample));
        if (stco_atom.tables == NULL)
        {
            printf("malloc failed.\n");
			free(stsz_atom.tables);
            goto Exit;
    	}
        for (i = 0; i < stco_atom.counts; i ++)
        {
            stco_atom.tables[i].offset = big_to_little_get_u32(buf + offset);
            offset += 4;
        }
        mov_print_stco(&stco_atom);
        
        pPBHandle->a_idx_array = (FrameInfo *)malloc(pPBHandle->a_idx_array_count * sizeof(FrameInfo));
        if (pPBHandle->a_idx_array == NULL)
        {
            printf("[%s][%d]pPBHandle->idx_array malloc failed.\n", __func__, __LINE__);
			free(stsz_atom.tables);
			free(stco_atom.tables);
            goto Exit;
        }

		pPBHandle->a_rate = mdhd_atom.time_scale;
		
        for(i = 0; i < pPBHandle->a_idx_array_count; i++)
        {
            pPBHandle->a_idx_array[i].size = stsz_atom.tables[i].size;
            pPBHandle->a_idx_array[i].offset = stco_atom.tables[i].offset;
            pPBHandle->a_idx_array[i].no++;
        }

        if (stsz_atom.tables)
        {
            free(stsz_atom.tables);
            stsz_atom.tables = NULL;
        }
        
        if (stco_atom.tables)
        {
            free(stco_atom.tables);
            stco_atom.tables = NULL;
        }

	}
    #endif

    /* udat */
    MovAtomHeader udta = {0}; //udta
    udta.size = big_to_little_get_u32(buf + offset);
    offset += 4;
    udta.type = big_to_little_get_u32(buf + offset);
    offset += 4;
    mov_print_atom_header(&udta, "udta");
    mov_print_udta(buf + offset, udta.size - 8);

Exit:		
	free(mov_buf);
    return 0;
}

MovPBHandle *AR_MOV_CreatePbHandle(void)
{
    MovPBHandle *pPBHandle = NULL;
    pPBHandle = (MovPBHandle *)malloc(sizeof(MovPBHandle));
    if(NULL == pPBHandle)
    {
    	printf("[%s][%d] malloc failed\n ", __func__,__LINE__);
        return NULL;
    }
    memset(pPBHandle, 0, sizeof(MovPBHandle));
    pPBHandle->a_idx_array = NULL;
    pPBHandle->file = NULL;
    pPBHandle->idx_array = NULL;
	pPBHandle->pb_buf = NULL;
	pPBHandle->a_pb_buf = NULL;

    return pPBHandle;
}

int AR_MOV_LoadFile(char *file_name, MovPBHandle *pPBHandle)
{
    int ret;

    if(!pPBHandle || !file_name)
    {
        printf("parameter is error.\n");
        return -1;
    }

    if(pPBHandle->file)
    {
        fclose(pPBHandle->file);
        pPBHandle->file = NULL;
    }
	
    if(pPBHandle->idx_array)
    {
        free(pPBHandle->idx_array);
        pPBHandle->idx_array = NULL;
    }

	if(pPBHandle->a_idx_array)
    {
        free(pPBHandle->a_idx_array);
        pPBHandle->a_idx_array = NULL;
    }

    //open file
    pPBHandle->file = fopen(file_name, "rb+");
    if(NULL == pPBHandle->file)
    {
        printf("open %s failed.\n", file_name);
        return -1;
    }

    //read mov header
    ret = mov_read_header(pPBHandle);
    if(ret < 0)
    {
        printf("call mov_read_header failed.\n");
        return -1;
    }

    printf("mov_pb_open ok.\n");
    return 0;
}


MovPBHandle *AR_MOV_ReadPbHandleCreate(char *filename)
{
    printf("call avi_pb_open_by_file\n");

    MovPBHandle *pPBHandle = AR_MOV_CreatePbHandle();
    if (NULL == pPBHandle) {
		printf("[%s][%d] AR_MOV_CreatePbHandle failed\n ", __func__, __LINE__);
        return NULL;
    }

    int ret = AR_MOV_LoadFile(filename, pPBHandle);
    if (ret < 0) {
        printf("avi_pb_open %s error.\n", filename);
        return NULL;
    }

    return pPBHandle;
}

AR_S32 AR_MOV_ReadPbHandleDestory(MovPBHandle *pPBHandle)
{
    if(!pPBHandle)
    {
        printf("parameter is error.\n");
        return -1;
    }

    //close & free
    if(pPBHandle->file)
    {
        fclose(pPBHandle->file);
        pPBHandle->file = NULL;
    }
	
    if(pPBHandle->idx_array)
    {
        free(pPBHandle->idx_array);
        pPBHandle->idx_array = NULL;
    }
	
    if(pPBHandle->pb_buf)
    {
        free(pPBHandle->pb_buf);
        pPBHandle->pb_buf = NULL;
    }

	if(pPBHandle->a_idx_array)
    {
        free(pPBHandle->a_idx_array);
        pPBHandle->a_idx_array = NULL;
    }
	
    if(pPBHandle->a_pb_buf)
    {
        free(pPBHandle->a_pb_buf);
        pPBHandle->a_pb_buf = NULL;
    }

    free(pPBHandle);
    
    return 0;
}


AR_S32 AR_MOV_ReadGetVideoFrame(MovPBHandle *pPBHandle)
{
    int ret;
	int bytes_loop = 0;
	int offset = 0;

	//read fram no > file frame no
    if(pPBHandle->video_no >= pPBHandle->idx_array_count)
    {
        printf("mov file has been read to the end, please return to next\n");
        return -1;
    }

    bytes_loop = pPBHandle->idx_array[pPBHandle->video_no].size;
	if(bytes_loop >= pPBHandle->pb_buf_size)
    {
        if(pPBHandle->pb_buf == NULL)
        {
            pPBHandle->pb_buf = malloc(bytes_loop + 60*1024);
            if(pPBHandle->pb_buf == NULL)
            {
                printf("Malloc read frame buffer(%dKB) failed",(bytes_loop+60*1024)>>10);
                pPBHandle->pb_buf_size = 0;
                return -1;
            }
        }
        else
        {
            pPBHandle->pb_buf = realloc(pPBHandle->pb_buf, bytes_loop + 60*1024);
            if(pPBHandle->pb_buf == NULL)
            {
                printf("Realloc read frame buffer(%dKB) failed",(bytes_loop+60*1024)>>10);
                pPBHandle->pb_buf_size = 0;
                return -1;
            }
        
            printf("Remalloc read frame buffer(%dKB) ",(bytes_loop+60*1024)>>10);
        }
        pPBHandle->pb_buf_size = (bytes_loop + 60*1024);
	}

	offset = pPBHandle->idx_array[pPBHandle->video_no].size;
	fseek(pPBHandle->file, offset, SEEK_SET);
	ret = fread(pPBHandle->pb_buf, 1, bytes_loop, pPBHandle->file);
	if(ret != bytes_loop)
	{
    	printf("[%s][%d]read failed. ret = %d\n", __func__, __LINE__, ret);
    	return -1;
	}

	pPBHandle->node.size = bytes_loop;
	pPBHandle->node.one_frame_buf = pPBHandle->pb_buf;
	pPBHandle->video_no++;
    return 0;
 }

AR_S32 AR_MOV_ReadGetAudioFrame(MovPBHandle *pPBHandle)
{
    int ret;
	int bytes_loop = 0;
	int offset = 0;

	//read fram no > file frame no
    if(pPBHandle->audio_no >= pPBHandle->a_idx_array_count)
    {
        printf("mov file has been read to the end, please return to next\n");
        return -1;
    }

    bytes_loop = pPBHandle->a_idx_array[pPBHandle->audio_no].size;
	if(bytes_loop >= pPBHandle->a_pb_buf_size)
    {
        if(pPBHandle->a_pb_buf == NULL)
        {
            pPBHandle->a_pb_buf = malloc(bytes_loop + 60*1024);
            if(pPBHandle->a_pb_buf == NULL)
            {
                printf("Malloc read frame buffer(%dKB) failed",(bytes_loop+60*1024)>>10);
                pPBHandle->a_pb_buf_size = 0;
                return -1;
            }
        }
        else
        {
            pPBHandle->a_pb_buf = realloc(pPBHandle->a_pb_buf, bytes_loop + 60*1024);
            if(pPBHandle->a_pb_buf == NULL)
            {
                printf("Realloc read frame buffer(%dKB) failed",(bytes_loop+60*1024)>>10);
                pPBHandle->a_pb_buf_size = 0;
                return -1;
            }
        
            printf("Remalloc read frame buffer(%dKB) ",(bytes_loop+60*1024)>>10);
        }
        pPBHandle->a_pb_buf_size = (bytes_loop + 60*1024);
	}

	offset = pPBHandle->a_idx_array[pPBHandle->audio_no].size;
	fseek(pPBHandle->file, offset, SEEK_SET);
	ret = fread(pPBHandle->a_pb_buf, 1, bytes_loop, pPBHandle->file);
	if(ret != bytes_loop)
	{
    	printf("[%s][%d]read failed. ret = %d\n", __func__, __LINE__, ret);
    	return -1;
	}

	pPBHandle->a_node.size = bytes_loop;
	pPBHandle->a_node.one_frame_buf = pPBHandle->a_pb_buf;
	pPBHandle->audio_no++;
    return 0;
 }


