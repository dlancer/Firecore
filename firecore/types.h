#ifndef _TYPES_H
#define _TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//
typedef struct { // 4 bytes RAM
  uint8_t x;
  uint8_t y;
} position_t;

typedef struct {  // need for collision check
  position_t Base;
  position_t New;
} objPosition_t;

typedef struct {  // 5 bytes RAM
  position_t pos;
  uint8_t color; // as we use NES palette, store only color id
} stars_t;

typedef struct {  // 5 bytes RAM
  position_t pos;
  struct {
    uint8_t onUse   :1;
    uint8_t state   :1;
    uint8_t freeRam :6;
  };
} rocket_t;

typedef struct {  // 8 bytes RAM
  objPosition_t pos;
  bool state;
} gift_t;

typedef struct {
  int8_t bombsLeft;
  int16_t rocketsLeft;
  struct {
    uint8_t overHeated    :1;
    uint8_t state         :1;
    //uint8_t bombsUnlocked :1;
    uint8_t freeRam       :6;
  };
} weapon_t;

typedef struct {
  int8_t speed;
  int8_t power;
  int8_t durability;
} shipStats_t;

typedef struct {  // 8 + 5 + 5 bytes RAM
  objPosition_t pos;
  weapon_t weapon;
  shipStats_t states;
  int16_t health;
} ship_t;

typedef struct {  // 8 + 6 + 5 bytes RAM
  objPosition_t pos;
  struct {
    uint8_t state   :1;     // Pic what we draw
    uint8_t alive   :1;
    uint8_t freeRam :6;
  };
  int16_t health;
  uint16_t timeToShoot;
  uint8_t respawnTime;  // 255 sec will be enougth... If call checkInVadersRespawn 1 time in sec...
  rocket_t deathRay;
} inVader_t;
/*
typedef struct {
  inVader_t alienBoss;
  rocket_t deathRays[4];
} inVaderBoss_t;
*/

typedef struct {  // 1 byte RAM
  struct {
    uint8_t updLife  :1;
    uint8_t updPew   :1;
    //uint8_t updScore     :1;
    uint8_t freeRam      :6;
  };
} hudStatus_t;

typedef struct {
  union {
  	uint8_t zBtn; // for fast reset only
  	struct {
  	  uint8_t aBtn :1;
  	  uint8_t bBtn :1;
  	  uint8_t xBtn :1;
  	  uint8_t yBtn :1;
  	  uint8_t freeRam :4;
    };
  };
} btnStatus_t;

typedef struct {
  uint8_t name[4]; // 3 for name 4 for '\n'
  uint16_t score;  // 65535 score val be enought right?
} saveData_t;


typedef struct {
  pFunc_t task;
  uint16_t timeout;
} taskParams_t;

typedef const taskParams_t * const tasksArr_t;

//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_COMMON_H*/
