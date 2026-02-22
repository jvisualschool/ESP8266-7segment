# ESP8266 Pixel Animation System — MAX7219 + OLED

ESP8266 기반의 임베디드 픽셀 애니메이션 시스템입니다.  
7-세그먼트 형태의 MAX7219 8-Digit 모듈을 픽셀 매트릭스로 운용하며, 50종의 독립적인 프레임 애니메이션을 순차적으로 실행합니다.  
I2C 기반 OLED는 각 애니메이션의 메타데이터(번호, 제목, 설명)를 별도 채널로 표시합니다.

---

## Screenshots

| 하드웨어 동작 | 웹 시뮬레이터 | 애니메이션 목록 |
| :---: | :---: | :---: |
| ![Hardware](https://jvibeschool.org/ESP8266-7segment/screenshot1.png) | ![Simulator](https://jvibeschool.org/ESP8266-7segment/screenshot2.png) | ![Playlist](https://jvibeschool.org/ESP8266-7segment/screenshot3.png) |

---

## Hardware Overview

| 구성 요소 | 역할 |
| :--- | :--- |
| **ESP8266** (NodeMCU / Wemos D1 Mini) | 시스템 제어 MCU. SPI(MAX7219), I2C(OLED) 동시 구동 |
| **MAX7219 8-Digit Display** | 메인 디스플레이. 8×8 세그먼트 비트맵 렌더링 |
| **0.96" I2C OLED** (SSD1306, 128×64) | 보조 정보 채널. 애니메이션 인덱스 및 설명 출력 |

---

## System Architecture

### 1. Display Buffer (`state[8]`)

MAX7219에 직접 쓰기 연산을 수행하지 않습니다.  
`byte state[8]` 배열을 추상화된 디스플레이 버퍼로 사용하여, 각 프레임의 렌더링 로직을 버퍼에 선(先) 연산한 뒤 `drawState()` 함수를 통해 일괄 플러시(flush)합니다.  
이를 통해 부분 갱신으로 인한 화면 티어링(tearing)을 제거하였습니다.

### 2. 애니메이션 디스패처 (Function Pointer + Struct)

50개의 애니메이션 로직을 `switch-case` 없이 관리하기 위해, 구조체 기반의 플레이리스트 테이블을 채택하였습니다.

```cpp
struct AnimRule {
    const char* title;
    const char* description;
    void (*func)();
};

AnimRule playlist[50] = { ... };
```

`loop()` 함수는 `playlist` 배열을 순회하며, 현재 인덱스에 해당하는 함수 포인터를 호출합니다.  
이 구조를 통해 각 애니메이션은 독립적인 실행 단위로 분리되어, 추가·교체·테스트가 용이합니다.

### 3. 비동기 타이머 (`millis()` 기반)

`delay()` 함수는 MCU 블로킹을 유발하므로, 모든 프레임 타이밍은 `millis()` 기반의 논블로킹(non-blocking) 타이머로 처리합니다.  
각 애니메이션은 **6초** 동안 실행됩니다.

### 4. 비트 연산 기반 세그먼트 제어

8 비트(7 세그먼트 + DP) 구조를 이진 연산으로 직접 제어합니다.

| 연산자 | 용도 |
| :---: | :--- |
| `\|` (OR) | 특정 세그먼트 점등 |
| `& ~` (AND NOT) | 특정 세그먼트 소등 |
| `^` (XOR) | 세그먼트 토글 |
| `<<` (SHIFT) | 패턴 이동 |

### 5. 부팅 시퀀스

1. **AWAKENING** : 전체 픽셀 점등 후 6초간 점진적 밝기 사이클 (워밍업)
2. **COUNTDOWN** : 블록 방식의 초 카운트다운 시퀀스
3. **Main Loop** : `playlist[0]` ~ `playlist[49]` 순환 실행

---

## Wiring

**OLED — I2C**

| OLED 핀 | ESP8266 핀 |
| :---: | :---: |
| VCC | 3.3V |
| GND | GND |
| SDA | D2 (GPIO 4) |
| SCL | D1 (GPIO 5) |

**MAX7219 — SPI**

| MAX7219 핀 | ESP8266 핀 |
| :---: | :---: |
| VCC | 5V (Vin) |
| GND | GND |
| DIN | D7 (GPIO 13) |
| CS | D8 (GPIO 15) |
| CLK | D5 (GPIO 14) |

> MAX7219는 구동 전압으로 5V를 권장합니다.  
> 두 디스플레이의 전원 핀을 분리하여 연결하면 전원 노이즈를 줄일 수 있습니다.

---

## Dependencies

Arduino IDE의 라이브러리 매니저에서 아래 라이브러리를 설치합니다.

| 라이브러리 | 작성자 | 용도 |
| :--- | :--- | :--- |
| `LedControl` | Eberhard Fahle | MAX7219 SPI 통신 및 세그먼트 제어 |
| `Adafruit SSD1306` | Adafruit | OLED 하드웨어 드라이버 |
| `Adafruit GFX Library` | Adafruit | 2D 그래픽 렌더링 엔진 |

---

## Build & Upload

1. `ESP8266-MAX7219.ino` 파일을 Arduino IDE에서 엽니다.
2. **Tools > Board** 에서 사용 중인 ESP8266 보드를 선택합니다.
3. **Tools > Port** 에서 연결된 포트를 선택합니다.
4. **Upload** 를 실행합니다.

---

## Web Simulator (`index.html`)

하드웨어 없이 동작을 확인할 수 있는 브라우저 기반 시뮬레이터가 포함되어 있습니다.  
`index.html` 파일을 브라우저로 열면, 펌웨어와 동일한 로직 및 타이밍으로 50종의 애니메이션이 HTML5 Canvas 위에서 실행됩니다.  
별도의 서버나 빌드 과정 없이 로컬에서 즉시 구동 가능합니다.

---

## Animation Playlist

부팅 완료 후 아래 50종의 애니메이션이 각 6초 간격으로 순차 실행됩니다.

| # | 이름 | 설명 |
| :-: | :--- | :--- |
| 01 | BIG BOX | 외곽 테두리를 점진적으로 완성하는 사각형 드로잉 |
| 02 | PING PONG | 좌우 벽면 사이를 왕복하는 단일 도트 바운싱 |
| 03 | SNAKE | 외곽 경계를 따라 연속 이동하는 선형 체인 |
| 04 | SCAN | 상단에서 하단으로 진행하는 수평 스캔 라인 |
| 05 | ELEVATOR | 중앙 기준으로 개폐되는 슬라이딩 패널 |
| 06 | WAVE | 정현파(Sine) 기반의 위상 이동 파형 |
| 07 | GALAXY | 무작위 위치에서 발생하는 픽셀 점멸 패턴 |
| 08 | BREATHING | 사각형이 팽창·수축을 반복하는 진폭 사이클 |
| 09 | RAIN DROP | 상단에서 하단으로 낙하 후 반사되는 도트 시뮬레이션 |
| 10 | ZIGZAG | 사선 방향으로 진행하는 계단형 스윕 |
| 11 | GEARS | 복수의 세그먼트가 교차 회전하는 기어 패턴 |
| 12 | LOADING | 도트가 순차적으로 채워지는 진행 표시 |
| 13 | EKG LINE | 주기적 스파이크를 포함한 심전도 파형 |
| 14 | PAC-MAN | 도트를 소비하며 이동하는 아케이드 참조 패턴 |
| 15 | VOLUME EQ | 각 열 독립 진폭을 가진 이퀄라이저 스펙트럼 |
| 16 | CASINO | 열별 순차 고정되는 슬롯 릴(Reel) 시뮬레이션 |
| 17 | INCHWORM | 수축·신장을 반복하며 전진하는 선형 이동 |
| 18 | EYES | 수평 이동 및 점멸을 포함한 안구 추적 패턴 |
| 19 | LASER | 충전 후 전방 방출되는 에너지 빔 시퀀스 |
| 20 | TETRIS | 상단에서 낙하하여 누적되는 블록 쌓기 |
| 21 | PARTY | 각기 다른 속도와 위상으로 독립 회전하는 스피너 세트 |
| 22 | WIPER | 왕복 운동하는 수직 스윕 라인 |
| 23 | PULSE | 중심에서 외곽으로 확산되는 동심원 파장 |
| 24 | MATRIX | 세로 방향으로 흐르는 이진 데이터 스트림 |
| 25 | DOMINOES | 순차적으로 쓰러지는 수직 막대 체인 |
| 26 | TYPING | 타자기 출력 방식으로 문자를 순차 표시 |
| 27 | TRAFFIC | 3단계 신호 상태를 주기적으로 전환 |
| 28 | ALIEN UFO | 수평 이동하며 하방 빔을 방출하는 비행체 |
| 29 | FIREWORK | 상승 후 방사형으로 확산되는 폭발 시퀀스 |
| 30 | STROBE | 고빈도 전체 픽셀 점멸 |
| 31 | BOUNCING | 양쪽 벽면 사이를 튕기는 단일 볼 물리 시뮬레이션 |
| 32 | SCANNER | 좌에서 우로 순차 점등되는 수평 레이더 스캔 |
| 33 | HEARTBEAT | 이중 펄스 구조의 심박 파형 재현 |
| 34 | WINDMILL | 중심축 기준으로 회전하는 블레이드 패턴 |
| 35 | RAIN MATRIX | 세로 방향으로 낙하하는 랜덤 도트 스트림 |
| 36 | VU METER 2 | 중앙 기준으로 대칭 확장·수축하는 레벨 미터 |
| 37 | RANDOM NOISE | 픽셀 단위의 무작위 점멸로 표현한 노이즈 패턴 |
| 38 | RINGS | 내부에서 외부로 순차 확산되는 사각형 링 |
| 39 | CROSSFIRE | 양쪽 끝에서 출발해 중앙에서 교차하는 빔 충돌 |
| 40 | RADAR SWEEP | 고정 축을 기준으로 회전하는 스윕 라인 |
| 41 | TYPE NUMBERS | 각 자리에 순차적으로 숫자 세그먼트를 채우는 인덱싱 |
| 42 | MORSE CODE | 단신·장신 펄스 조합으로 표현하는 모스 부호 신호 |
| 43 | WORMHOLE | 외부 선이 중심부로 수렴하는 블랙홀 흡수 시퀀스 |
| 44 | HYPNOTIC | 교차 반전되는 빗살 패턴의 주기적 전환 |
| 45 | METEOR | 대각선 방향으로 이동하는 유성우 도트 트레일 |
| 46 | PINBALL | 예측 불가능한 벡터로 반사되는 핀볼 시뮬레이션 |
| 47 | PENDULUM | 좌우 진자 운동을 물리 기반으로 재현 |
| 48 | BARBER POLE | 대각선 줄무늬가 연속 이동하는 회전 착시 패턴 |
| 49 | SPARKLES | 무작위 위치에서 발생·소멸하는 독립 점멸 이벤트 |
| 50 | GLITCH | 비정상 비트 패턴을 삽입하여 표현한 데이터 손상 시뮬레이션 |
