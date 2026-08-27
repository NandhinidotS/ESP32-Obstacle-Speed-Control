// ======================================================
// ESP32 OBSTACLE DETECTION & AUTOMATIC SPEED CONTROL
// ======================================================

// ---------------- ULTRASONIC SENSOR ----------------
#define TRIG_PIN 21
#define ECHO_PIN 22

// ---------------- L298N MOTOR DRIVER ----------------
#define ENA 25
#define IN1 26
#define IN2 27

// ---------------- LED & BUZZER ----------------
#define GREEN_LED 18
#define RED_LED   5
#define BUZZER    19

// ---------------- ESP32 PWM ----------------
#define PWM_CHANNEL    0
#define PWM_FREQUENCY  1000
#define PWM_RESOLUTION 8


void setup()
{
  Serial.begin(115200);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // L298N
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // LED + Buzzer
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Motor direction
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // PWM setup
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNEL);

  // Initial condition
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  ledcWrite(PWM_CHANNEL, 255);

  Serial.println("================================");
  Serial.println("OBSTACLE SPEED CONTROL SYSTEM");
  Serial.println("================================");
}


// ======================================================
// DISTANCE MEASUREMENT
// ======================================================

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
  {
    return 999;
  }

  float distance = duration * 0.0343 / 2;

  return distance;
}


// ======================================================
// MAIN LOOP
// ======================================================

void loop()
{
  float distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");


  // ====================================================
  // CASE 1: DISTANCE > 20 CM
  // FULL SPEED
  // GREEN LED ON
  // ====================================================

  if (distance > 20)
  {
    ledcWrite(PWM_CHANNEL, 255);

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    Serial.println("PATH CLEAR - FULL SPEED");
  }


  // ====================================================
  // CASE 2: 10 CM TO 20 CM
  // SPEED GRADUALLY DECREASES
  // GREEN LED ON
  // ====================================================

  else if (distance >= 10)
  {
    // Convert:
    // 20 cm -> PWM 255
    // 10 cm -> PWM 0

    int speed = map(distance, 10, 20, 0, 255);

    // Prevent very low starting torque
    if (speed < 60)
    {
      speed = 60;
    }

    ledcWrite(PWM_CHANNEL, speed);

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    Serial.print("OBSTACLE DETECTED - SPEED: ");
    Serial.println(speed);
  }


  // ====================================================
  // CASE 3: BELOW 10 CM
  // MOTOR STOP
  // GREEN OFF
  // RED ON
  // BUZZER ON
  // ====================================================

  else
  {
    ledcWrite(PWM_CHANNEL, 0);

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);

    Serial.println("!!! STOP !!!");
    Serial.println("Obstacle below 10 cm");
  }


  Serial.println("--------------------------------");

  delay(100);
}
