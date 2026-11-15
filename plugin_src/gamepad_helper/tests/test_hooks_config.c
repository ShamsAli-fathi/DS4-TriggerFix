/* Host integration checks; test SDK stubs never enter the PS4 build. */
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include "../source/main.c"
static ScePadData samples[3];
static int read_result, state_result;
int scePadReadExt(int h, ScePadData *p, int n) {
    (void)h;
    if (read_result > 0) memcpy(p, samples, (size_t)n * sizeof(*p));
    return read_result;
}
int scePadReadStateExt(int h, ScePadData *p) {
    (void)h;
    if (state_result == 0) *p = samples[0];
    return state_result;
}
static void independent_triggers(void) {
    ScePadData got[3], expected[3];
    for (int mode = 0; mode < 4; ++mode) {
        for (int threshold = 0; threshold <= 30; threshold += 30) {
            memset(samples, 0x5a, sizeof(samples));
            for (int i = 0; i < 3; ++i) {
                samples[i].buttons = 0;
                samples[i].connected = i != 1;
                samples[i].analogButtons.l2 = i == 2 ? 64 : 128;
                samples[i].analogButtons.r2 = i == 2 ? 51 : 102;
            }
            memcpy(expected, samples, sizeof(expected));
            g_enableL2Scaling = (mode & 1) != 0;
            g_enableR2Scaling = (mode & 2) != 0;
            g_l2InputMax = 128; g_r2InputMax = 102;
            g_l2DigitalThreshold = g_r2DigitalThreshold = threshold;
            for (int i = 0; i < 3; ++i) {
                if (!samples[i].connected) continue;
                if (g_enableL2Scaling) {
                    expected[i].analogButtons.l2 = i == 2 ? 128 : 255;
                    if (threshold) expected[i].buttons |= SCE_PAD_BUTTON_L2;
                }
                if (g_enableR2Scaling) {
                    expected[i].analogButtons.r2 = i == 2 ? 128 : 255;
                    if (threshold) expected[i].buttons |= SCE_PAD_BUTTON_R2;
                }
            }
            read_result = 3; state_result = 0;
            assert(scePadRead_hook(1, got, 3) == 3);
            assert(memcmp(got, expected, sizeof(got)) == 0);
            assert(scePadReadState_hook(1, got) == 0);
            assert(memcmp(got, expected, sizeof(*got)) == 0);
            read_result = state_result = -1;
            memcpy(got, samples, sizeof(got));
            assert(scePadRead_hook(1, got, 3) == -1);
            assert(scePadReadState_hook(1, got) == -1);
            assert(memcmp(got, samples, sizeof(got)) == 0);
            read_result = 0;
            assert(scePadRead_hook(1, got, 3) == 0);
            assert(memcmp(got, samples, sizeof(got)) == 0);
        }
    }
    /* Synthesized digital bits must still pass through existing remapping. */
    uint32_t mappings[BUTTON_MAPPING_MAX] = {0};
    mappings[BUTTON_L2] = SCE_PAD_BUTTON_CROSS;
    mappings[BUTTON_R2] = SCE_PAD_BUTTON_SQUARE;
    buttonMapping = mappings; g_enableCustomButton = true;
    read_result = 1; state_result = 0;
    assert(scePadRead_hook(1, got, 1) == 1);
    assert(got[0].buttons == (SCE_PAD_BUTTON_CROSS | SCE_PAD_BUTTON_SQUARE));
    assert(scePadReadState_hook(1, got) == 0);
    assert(got[0].buttons == (SCE_PAD_BUTTON_CROSS | SCE_PAD_BUTTON_SQUARE));
    g_enableCustomButton = false; buttonMapping = NULL;
}

int main(void) {
    _Static_assert(offsetof(ScePadData, analogButtons.l2) == 8, "L2 ABI offset");
    _Static_assert(offsetof(ScePadData, analogButtons.r2) == 9, "R2 ABI offset");
    _Static_assert(sizeof(ScePadData) == 120, "pad sample ABI size");
    ScePadData got[3], expected[3];
    memset(samples, 0x5a, sizeof(samples));
    for (int i=0; i<3; ++i) {
        samples[i].connected = i != 1;
        samples[i].analogButtons.r2 = i == 2 ? 51 : 102;
        samples[i].analogButtons.l2 = 255;
    }
    memcpy(expected, samples, sizeof(expected));
    expected[0].analogButtons.r2 = 255;
    expected[2].analogButtons.r2 = 128;
    g_enableR2Scaling = true; g_r2InputMax=102; g_r2DigitalThreshold=0;
    g_enableDeadZone=g_enableCustomButton=g_enableCustomTouchPad=false;
    read_result=3;
    assert(scePadRead_hook(1, got, 3) == 3);
    assert(memcmp(got, expected, sizeof(got)) == 0);
    state_result=0;
    assert(scePadReadState_hook(1, got) == 0);
    assert(memcmp(got, expected, sizeof(*got)) == 0);
    g_enableR2Scaling=false;
    assert(scePadRead_hook(1, got, 3) == 3);
    assert(memcmp(got, samples, sizeof(got)) == 0);
    g_enableR2Scaling=true;
    read_result=-1; state_result=-1;
    assert(scePadRead_hook(1, got, 3) == -1);
    assert(scePadReadState_hook(1, got) == -1);
    assert(memcmp(got, samples, sizeof(got)) == 0);
    read_result=0;
    assert(scePadRead_hook(1, got, 3) == 0);
    assert(memcmp(got, samples, sizeof(got)) == 0);

    /* Exercise the existing file parser, with title before default. */
    char path[]="/tmp/gamepad-config-XXXXXX";
    int fd=mkstemp(path); assert(fd>=0);
    FILE *f=fdopen(fd,"w"); assert(f);
    fputs("[CUSA12345]\nenableR2Scaling=1\nR2InputMax=102\nR2DigitalThreshold=0\nenableL2Scaling=1\nL2InputMax=128\nL2DigitalThreshold=0\n"
          "[default]\nenableR2Scaling=0\nR2InputMax=200\nR2DigitalThreshold=30\nenableL2Scaling=0\nL2InputMax=200\nL2DigitalThreshold=30\n",f);
    fclose(f);
    ini_table_s *t=ini_table_create(); assert(t);
    assert(ini_table_read_from_file(t,path)); unlink(path);
    load_config(t,"default");
    assert(!g_enableL2Scaling && g_l2InputMax==200 && g_l2DigitalThreshold==30);
    assert(!g_enableR2Scaling && g_r2InputMax==200 && g_r2DigitalThreshold==30);
    load_config(t,"CUSA12345");
    assert(g_enableL2Scaling && g_l2InputMax==128 && g_l2DigitalThreshold==0);
    assert(g_enableR2Scaling && g_r2InputMax==102 && g_r2DigitalThreshold==0);
    const char *bad[]={"0","-1","256","nonsense"};
    for (unsigned i=0;i<4;++i) {
        ini_table_create_entry(t,"CUSA12345","R2InputMax",bad[i]);
        ini_table_create_entry(t,"CUSA12345","L2InputMax",bad[i]);
        load_config(t,"CUSA12345"); assert(g_r2InputMax==102 && g_l2InputMax==128);
    }
    ini_table_create_entry(t,"CUSA12345","R2DigitalThreshold","256");
    load_config(t,"CUSA12345"); assert(g_r2DigitalThreshold==0);
    ini_table_create_entry(t,"CUSA12345","L2DigitalThreshold","-1");
    load_config(t,"CUSA12345"); assert(g_l2DigitalThreshold==0);
    ini_table_create_entry(t,"CUSA12345","L2InputMax","255");
    ini_table_create_entry(t,"CUSA12345","L2DigitalThreshold","255");
    load_config(t,"CUSA12345");
    assert(g_l2InputMax==255 && g_l2DigitalThreshold==255);
    ini_table_destroy(t);
    independent_triggers();
    puts("PASS: independent L2/R2 and combined hooks, unrelated bytes, read errors, INI ranges and remapping");
    return 0;
}
