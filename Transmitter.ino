#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define JOYSTICK_LEFT_X A0
#define JOYSTICK_LEFT_Y A1

#define JOYSTICK_RIGHT_X A2
#define JOYSTICK_RIGHT_Y A3

#define JOYSTICK_LEFT_BUTTON 2
#define JOYSTICK_RIGHT_BUTTON 3

#define PIN_CE 7
#define PIN_CSN 8

// CE, CSN pins
RF24 radio(PIN_CE, PIN_CSN);

// Address (must match on both devices)
const byte address[6] = "00001";

struct Vec2i {
  int16_t x;
  int16_t y;
};

struct Input {
  Vec2i left;
  bool left_button;
  Vec2i right;
  bool right_button;
};

Input INPUT0 = { .left = { .x = 89 }, .left_button = 0, .right = { 0 }, .right_button = 12 };

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();  // Set as transmitter

  pinMode(JOYSTICK_LEFT_X, INPUT);
  pinMode(JOYSTICK_LEFT_Y, INPUT);
  pinMode(JOYSTICK_RIGHT_X, INPUT);
  pinMode(JOYSTICK_RIGHT_Y, INPUT);
  pinMode(JOYSTICK_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(JOYSTICK_RIGHT_BUTTON, INPUT_PULLUP);
}

void input_handle(Input *input) {
  input->left = { .x = analogRead(JOYSTICK_LEFT_X), .y = analogRead(JOYSTICK_LEFT_Y) };
  input->right = { .x = analogRead(JOYSTICK_RIGHT_X), .y = analogRead(JOYSTICK_RIGHT_Y) };
  input->left_button = digitalRead(JOYSTICK_LEFT_BUTTON) == LOW;
  input->right_button = digitalRead(JOYSTICK_RIGHT_BUTTON) == LOW;
}

void vec2i_print(const Vec2i *vec2i) {
  Serial.print("{.x = ");
  Serial.print(vec2i->x);
  Serial.print(", .y = ");
  Serial.print(vec2i->y);
  Serial.print("}");
}

void input_print(const Input *input) {
  Serial.print("{.left = ");
  vec2i_print(&input->left);
  Serial.print(", .right = ");
  vec2i_print(&input->right);
  Serial.print(", .left_button = ");
  Serial.print(input->left_button);
  Serial.print(", .right_button = ");
  Serial.print(input->right_button);
  Serial.println("}");
}

bool vec2i_cmp(const Vec2i *a, const Vec2i *b, uint8_t margin) {
  return abs(a->x - b->x) <= margin && abs(a->y - b->y) <= margin;
}

bool input_cmp(const Input *a, const Input *b, uint8_t margin) {
  return vec2i_cmp(&a->left, &b->left, margin) && vec2i_cmp(&a->right, &b->right, margin) && a->left_button == b->left_button && a->right_button == b->right_button;
}

Input prev_input = { 0 };

void loop() {
  Input input = { 0 };
  input_handle(&input);

  if (!input_cmp(&input, &prev_input, 5)) {
    input_print(&input);
    bool ok = radio.write(&input, sizeof(Input));
    if (ok) {
      Serial.println("Sent: Input");
    } else {
      Serial.println("Send failed");
    }
  }

  prev_input = input;
}