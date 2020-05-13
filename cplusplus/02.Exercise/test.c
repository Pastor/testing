#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SIZE 5
#define NUM_OF_STATES 5

/*Состояния*/
enum States {
    DET_AWAIT,              //ожидание материала
    MATERIAL_MOVE,          //движение конвеера (материала)
    STAMP_DOWN, STAMP_UP,   //движение штампа вниз/вверх
    STAMP_PROCESS,          //штамповка
    CUT_USELESS,            //отрезание израсходавнного участка материала
    ES
};

/*Выходные сигналы*/
enum Outputs {
    ZERO, E,    //ожидание/сигнал ошибки
    ST,         //вых. сигнал запуска
    SU, SD,     //штамп поднимается/опускается
    MC,         //срабатывание ножниц
    SP,         //процесс штамповки
    MM,         //материал движется
    MCZ         //материал в зоне штамовки
};

/*Набор корректных сигналов*/
const char correct_signals[NUM_OF_STATES][SIZE] = {{"1000"},
                                                   {"1100"},
                                                   {"1110"},
                                                   {"1011"},
                                                   {"1001"}};

/*Набор входящих сигналов*/
char signals[5][SIZE];

enum States current_state = DET_AWAIT;

int automat(void);

void messages(int current_output);

void read_signals(char *filename);

int main(int argc, const char *argv[]) {
    char *filename = "signals.txt";
    read_signals(filename);
    int output;
    while (1) {
        output = automat();
        messages(output);
        if (output == MC) {
            printf("System message: full cycle is done.\n");
            break;
        }
        if (output == E)
            break;
    }
    return 0;
}

void read_signals(char *filename) {
    FILE *fd = fopen(filename, "r");
    int field_num = 0;
    char *line;

    if (fd == 0) {
        printf("Can't open file: there is no file with this name.\n");
    } else {
        while (field_num != NUM_OF_STATES) {
            line = fgets(signals[field_num], sizeof(signals[field_num]) + 1, fd);
            if (field_num < NUM_OF_STATES - 1)
                signals[field_num][_mbstrlen(signals[field_num]) - 1] = '\0';
            ++field_num;
            if (line == NULL) {
                break;
            }
        }
    }
}

void messages(int current_output) {
    switch (current_output) {
        case ST:
            printf("Current state: material waiting.\n");
            break;
        case SU:
            printf("Current state: stamp moving up.\n");
            break;
        case SD:
            printf("Current state: stamp moving down.\n");
            break;
        case MC:
            printf("Current state: cut process.\n");
            break;
        case SP:
            printf("Current state: stamp process now.\n");
            break;
        case MM:
            printf("Current state: material moving.\n");
            break;
        case MCZ:
            printf("Current state: material in the stamp zone.\n");
            break;
        case E:
            printf("Current state: critical error. Check sensors!\n");
            break;
        default:
            break;
    }
}

int automat(void) {

    switch (current_state) {
        case DET_AWAIT: {
            if ((signals[0][0] == correct_signals[0][0]) && (signals[0][1] == correct_signals[0][1]) &&
                (signals[0][2] == correct_signals[0][2]) && (signals[0][3] == correct_signals[0][3])) {
                current_state = MATERIAL_MOVE;
                return ST;
            } else if (signals[0][0] == 0) {
                return ZERO;
            } else {
                current_state = ES;
                return E;
            }
        }
        case MATERIAL_MOVE:
            if ((signals[1][0] == correct_signals[1][0]) && (signals[1][1] == correct_signals[1][1]) &&
                (signals[1][2] == correct_signals[1][2]) && (signals[1][3] == correct_signals[1][3])) {
                current_state = STAMP_DOWN;
                return MCZ;
            } else {
                current_state = ES;
                return E;
            }
        case STAMP_DOWN:
            if ((signals[2][0] == correct_signals[2][0]) && (signals[2][1] == correct_signals[2][1]) &&
                (signals[2][2] == correct_signals[2][2]) && (signals[2][3] == correct_signals[2][3])) {
                current_state = STAMP_PROCESS;
                return SD;
            } else {
                current_state = ES;
                return E;
            }
        case STAMP_PROCESS:
            if ((signals[3][0] == correct_signals[3][0]) && (signals[3][1] == correct_signals[3][1]) &&
                (signals[3][2] == correct_signals[3][2]) && (signals[3][3] == correct_signals[3][3])) {
                current_state = STAMP_UP;
                return SU;
            } else {
                current_state = ES;
                return E;
            }
        case STAMP_UP:
            if ((signals[4][0] == correct_signals[4][0]) && (signals[4][1] == correct_signals[4][1]) &&
                (signals[4][2] == correct_signals[4][2]) && (signals[4][3] == correct_signals[4][3])) {
                current_state = CUT_USELESS;
                return MC;
            } else {
                current_state = ES;
                return E;
            }
        case CUT_USELESS:
            if ((signals[0][0] == correct_signals[1][0]) && (signals[0][1] == correct_signals[0][1]) &&
                (signals[0][2] == correct_signals[1][2]) && (signals[0][3] == correct_signals[0][3])) {
                current_state = MATERIAL_MOVE;
                return MM;
            } else {
                current_state = ES;
                return E;
            }
        case ES:
            return E;
        default:
            break;
    }
    return 1;
}
