#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

														  // include glm/*.hpp only if necessary
														  //#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define PI 3.14159265359f

float delta_time = 0.016f;

int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

// 필요 전역 변수
float background_brightness = 1.0f;  // 초기값 (밝음)

//road
#define ROAD_UPPER_LINE 0
#define ROAD_BODY 1
#define ROAD_LOWER_LINE 2
GLfloat road_upper_line[4][2] = { { -150.0, 110.0 },{ -150.0, 120.0 },{ 150.0, 110.0 },{ 150.0, 120.0 } };
GLfloat road_body[4][2] = { { -150.0, 10.0 },{ -150.0, 110.0 },{ 150.0, 10.0 },{ 150.0, 110.0 } };
GLfloat road_lower_line[4][2] = { { -150.0, 0.0 },{ -150.0, 10.0 },{ 150.0, 0.0 },{ 150.0, 10.0 } };

GLfloat road_color[9][3] = {
	{ 1.0f, 1.0f, 1.0f },
	{ 0.2f, 0.2f, 0.2f },
	{ 1.0f, 1.0f, 1.0f }
};

GLuint VBO_road, VAO_road;

int road_clock = 0;
float road_s_factor = 1.0f;

void prepare_road() {
	GLsizeiptr buffer_size = sizeof(road_upper_line) + sizeof(road_body) + sizeof(road_lower_line);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_road);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(road_upper_line), road_upper_line);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_upper_line), sizeof(road_body), road_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road_upper_line) + sizeof(road_body), sizeof(road_lower_line), road_lower_line);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_road);
	glBindVertexArray(VAO_road);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_road() { // Draw road in its MC.
	glBindVertexArray(VAO_road);

	glUniform3fv(loc_primitive_color, 1, road_color[ROAD_UPPER_LINE]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[ROAD_BODY]);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[ROAD_LOWER_LINE]);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

	glBindVertexArray(0);
}

//soldier
#define SOLDIER_HEAD 0
#define SOLDIER_EYE 1
#define SOLDIER_EAR 2
#define SOLDIER_MOUTH 3
#define SOLDIER_NECK 4
#define SOLDIER_VEST 5
#define SOLDIER_ARM 6
#define SOLDIER_PANT 7
#define SOLDIER_BOOT 8
#define SOLDIER_CLOAK 9
GLfloat soldier_head[7][2] = { { 0.0, 90.0 },{ 0.0, 40.0 },{ 10.0, 45.0 },{ 25.0, 55.0 },{ 25.0, 70.0 },{ 20.0, 80.0 },{ 10.0, 90.0 } };
GLfloat soldier_eye[4][2] = { { 5.0, 65.0 },{ 5.0, 60.0 },{ 10.0, 60.0 },{ 10.0, 65.0 } };
GLfloat soldier_ear[4][2] = { { 25.0, 65.0 },{ 25.0, 55.0 },{ 30.0, 60.0 },{ 30.0, 65.0 } };
GLfloat soldier_mouth[3][2] = { { 0.0, 50.0 },{ 0.0, 45.0 },{ 5.0, 50.0 } };
GLfloat soldier_neck[3][2] = { { 0.0, 40.0 },{ 0.0, 35.0 },{ 10.0, 45.0 } };
GLfloat soldier_vest[5][2] = { { 0.0, 35.0 },{ 0.0, 0.0 },{ 25.0, 0.0 },{ 25.0, 45.0 },{ 10.0, 45.0 } };
GLfloat soldier_arm[4][2] = { { 25.0, 45.0 },{ 25.0, 30.0 },{ 35.0, 15.0 },{ 45.0, 10.0 } };
GLfloat soldier_pant[6][2] = { { 0.0, 0.0 },{ 0.0, -15.0 },{ 5.0, -15.0 },{ 5.0, -30.0 },{ 25.0, -30.0 },{ 25.0, 0.0 } };
GLfloat soldier_boot[5][2] = { { 5.0, -30.0 },{ 5.0, -45.0 },{ 35.0, -45.0 },{ 25.0, -40.0 },{ 25.0, -30.0 } };
GLfloat soldier_cloak[4][2] = { { 25.0, 30.0 },{ 25.0, -15.0 },{ 40.0, -15.0 },{ 30.0, 20.0 } };

GLfloat soldier_color[10][3] = {
	{ 1.0f, 210 / 255.0f, 170 / 255.0f },          // HEAD (살구색)
	{ 0.0f, 0.0f, 0.0f },                          // EYE (검정)
	{ 1.0f, 210 / 255.0f, 170 / 255.0f },          // EAR (살구색)
	{ 200 / 255.0f, 50 / 255.0f, 50 / 255.0f },    // MOUTH (붉은색)
	{ 1.0f, 210 / 255.0f, 170 / 255.0f },          // NECK (살구색)
	{ 70 / 255.0f, 85 / 255.0f, 55 / 255.0f },     // VEST (연군복색)
	{ 1.0f, 210 / 255.0f, 170 / 255.0f },          // ARM (살구색)
	{ 50 / 255.0f, 60 / 255.0f, 40 / 255.0f },     // PANT (진군복색)
	{ 101 / 255.0f, 67 / 255.0f, 33 / 255.0f },     // BOOT (갈색)
	{ 200 / 255.0f, 50 / 255.0f, 50 / 255.0f }    // CLOAK (붉은색)
};

GLuint VBO_soldier, VAO_soldier;

int soldier_clock = 0;
float soldier_s_factor = 1.0f;

void prepare_soldier() {
	GLsizeiptr buffer_size = sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear) + sizeof(soldier_mouth)
		+ sizeof(soldier_neck) + sizeof(soldier_vest) + sizeof(soldier_arm) + sizeof(soldier_pant) + sizeof(soldier_boot) + sizeof(soldier_cloak);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_soldier);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_soldier);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(soldier_head), soldier_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head), sizeof(soldier_eye), soldier_eye);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye), sizeof(soldier_ear), soldier_ear);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear), sizeof(soldier_mouth), soldier_mouth);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear) + sizeof(soldier_mouth),
		sizeof(soldier_neck), soldier_neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear) + sizeof(soldier_mouth) + sizeof(soldier_neck), sizeof(soldier_vest), soldier_vest);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear) + sizeof(soldier_mouth) + sizeof(soldier_neck) + sizeof(soldier_vest), sizeof(soldier_arm), soldier_arm);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear) + sizeof(soldier_mouth) + sizeof(soldier_neck) + sizeof(soldier_vest) + sizeof(soldier_arm), sizeof(soldier_pant), soldier_pant);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear) + sizeof(soldier_mouth) + sizeof(soldier_neck) + sizeof(soldier_vest) + sizeof(soldier_arm) + sizeof(soldier_pant), sizeof(soldier_boot), soldier_boot);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(soldier_head) + sizeof(soldier_eye) + sizeof(soldier_ear) + sizeof(soldier_mouth) + sizeof(soldier_neck) + sizeof(soldier_vest) + sizeof(soldier_arm) + sizeof(soldier_pant) + sizeof(soldier_boot), sizeof(soldier_cloak), soldier_cloak);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_soldier);
	glBindVertexArray(VAO_soldier);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_soldier);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_soldier() { // Draw soldier in its MC.
	glBindVertexArray(VAO_soldier);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 7);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_EAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_MOUTH]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 3);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 18, 3);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_VEST]);
	glDrawArrays(GL_TRIANGLE_FAN, 21, 5);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_ARM]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 4);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_PANT]);
	glDrawArrays(GL_TRIANGLE_FAN, 30, 6);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_BOOT]);
	glDrawArrays(GL_TRIANGLE_FAN, 36, 5);

	glUniform3fv(loc_primitive_color, 1, soldier_color[SOLDIER_CLOAK]);
	glDrawArrays(GL_TRIANGLE_FAN, 41, 4);

}

// pet
#define PET_BODY 0
#define PET_LEFT_LEG 1
#define PET_RIGHT_LEG 2
#define PET_TAIL 3
#define PET_HEAD 4
#define PET_LEFT_EAR 5
#define PET_RIGHT_EAR 6
#define PET_LEFT_EYE 7
#define PET_RIGHT_EYE 8

GLfloat pet_body[4][2] = { { -18.0, 18.0 }, { -18.0, 6.0 },{ 18.0, 6.0 }, { 18.0, 18.0 } };
GLfloat pet_left_leg[4][2] = { { -18.0, 6.0 },{ -18.0, 0.0 },{ -9.0, 0.0 },{ -9.0, 6.0 } };
GLfloat pet_right_leg[4][2] = { { 9.0, 6.0 },{ 9.0, 0.0 },{ 18.0, 0.0 },{ 18.0, 6.0 } };
GLfloat pet_tail[3][2] = { { -18.0, 30.0 },{ -18.0, 18.0 },{ -12.0, 18.0 } };
GLfloat pet_head[8][2] = { { 9.0, 30.0 },{ 9.0, 15.0 },{ 30.0, 15.0 }, { 21.0, 19.5 }, { 30.0, 21.0 }, { 30.0, 27.0 }, { 24.0, 27.0 }, { 24.0, 30.0 } };
GLfloat pet_left_ear[3][2] = { { 9.0, 39.0 },{ 9.0, 30.0 },{ 12.0, 30.0 } };
GLfloat pet_right_ear[3][2] = { { 24.0, 39.0 },{ 21.0, 30.0 },{ 24.0, 30.0 } };
GLfloat pet_left_eye[4][2] = { { 12.0, 27.0 },{ 12.0, 24.0 },{ 15.0, 24.0 },{ 15.0, 27.0 } };
GLfloat pet_right_eye[4][2] = { { 18.0, 27.0 },{ 18.0, 24.0 },{ 21.0, 24.0 },{ 21.0, 27.0 } };

GLfloat pet_color[9][3] = {
	{ 0.75f, 0.55f, 0.25f },   // 몸통
	{ 0.75f, 0.55f, 0.25f },   // 왼다리
	{ 0.75f, 0.55f, 0.25f },   // 오른다리
	{ 0.75f, 0.55f, 0.25f },   // 꼬리
	{ 0.75f, 0.55f, 0.25f },   // 머리
	{ 0.75f, 0.55f, 0.25f },   // 왼귀
	{ 0.75f, 0.55f, 0.25f },   // 오른귀
	{ 0.0f, 0.0f, 0.0f },     // 왼눈
	{ 0.0f, 0.0f, 0.0f }      // 오른눈
};

GLuint VBO_pet, VAO_pet;

void prepare_pet() {
	GLsizeiptr buffer_size = sizeof(pet_body) + sizeof(pet_left_leg) + sizeof(pet_right_leg) + sizeof(pet_tail) +
		sizeof(pet_head) + sizeof(pet_left_ear) + sizeof(pet_right_ear) +
		sizeof(pet_left_eye) + sizeof(pet_right_eye);

	glGenBuffers(1, &VBO_pet);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pet);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	int offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_body), pet_body); offset += sizeof(pet_body);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_left_leg), pet_left_leg); offset += sizeof(pet_left_leg);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_right_leg), pet_right_leg); offset += sizeof(pet_right_leg);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_tail), pet_tail); offset += sizeof(pet_tail);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_head), pet_head); offset += sizeof(pet_head);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_left_ear), pet_left_ear); offset += sizeof(pet_left_ear);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_right_ear), pet_right_ear); offset += sizeof(pet_right_ear);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_left_eye), pet_left_eye); offset += sizeof(pet_left_eye);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(pet_right_eye), pet_right_eye);

	glGenVertexArrays(1, &VAO_pet);
	glBindVertexArray(VAO_pet);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pet);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_pet() {
	glBindVertexArray(VAO_pet);
	int start = 0;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 4); start += 4;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_LEFT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 4); start += 4;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_RIGHT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 4); start += 4;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_TAIL]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 3); start += 3;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 8); start += 8;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_LEFT_EAR]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 3); start += 3;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_RIGHT_EAR]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 3); start += 3;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_LEFT_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 4); start += 4;

	glUniform3fv(loc_primitive_color, 1, pet_color[PET_RIGHT_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, start, 4);

	glBindVertexArray(0);
}


//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house(glm::vec3 body_color, glm::vec3 window_color) {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, &body_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, &window_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 },{ -18.0, 0.0 },{ -13.0, 0.0 },{ -10.0, 8.0 },{ 10.0, 8.0 },{ 13.0, 0.0 },{ 18.0, 0.0 },{ 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 },{ -8.0, 6.0 },{ -2.0, 6.0 },{ -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 },{ 0.0, 6.0 },{ 8.0, 6.0 },{ 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 },{ -13.0, -8.0 },{ -13.0, -7.0 },{ -11.0, -4.0 },{ -7.0, -4.0 },{ -5.0, -7.0 },{ -5.0, -8.0 },{ -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 },{ 5.0, -8.0 },{ 5.0, -7.0 },{ 7.0, -4.0 },{ 11.0, -4.0 },{ 13.0, -7.0 },{ 13.0, -8.0 },{ 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 },{ -17.0, -2.0 },{ -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 },{ -17.0, -5.0 },{ -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1) + sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

// hat
#define HAT_LEAF 0
#define HAT_BODY 1
#define HAT_STRIP 2
#define HAT_BOTTOM 3

GLfloat hat_leaf[4][2] = { { 3.0, 20.0 },{ 3.0, 28.0 },{ 9.0, 32.0 },{ 9.0, 24.0 } };
GLfloat hat_body[4][2] = { { -19.5, 2.0 },{ 19.5, 2.0 },{ 15.0, 20.0 },{ -15.0, 20.0 } };
GLfloat hat_strip[4][2] = { { -20.0, 0.0 },{ 20.0, 0.0 },{ 19.5, 2.0 },{ -19.5, 2.0 } };
GLfloat hat_bottom[4][2] = { { 25.0, 0.0 },{ -25.0, 0.0 },{ -25.0, -4.0 },{ 25.0, -4.0 } };

GLfloat hat_color[4][3] = {
	{ 167 / 255.0f, 255 / 255.0f, 55 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f },
{ 255 / 255.0f, 40 / 255.0f, 33 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f }
};

GLuint VBO_hat, VAO_hat;

void prepare_hat() {
	GLsizeiptr buffer_size = sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip) + sizeof(hat_bottom);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hat_leaf), hat_leaf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf), sizeof(hat_body), hat_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body), sizeof(hat_strip), hat_strip);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip), sizeof(hat_bottom), hat_bottom);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_hat);
	glBindVertexArray(VAO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_hat() {
	glBindVertexArray(VAO_hat);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_LEAF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_STRIP]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glBindVertexArray(0);
}

// sword

#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

// hat variables
glm::vec2 hat_cur_center;
glm::vec2 hat_prev_center;
glm::vec2 hat_nxt_center;
float hat_cur_radius = 60.0f;           // 회전 반지름
float hat_cur_angle = 0.0f;             // 회전 각도 (라디안)
float hat_angle_speed = 0.5f;           // 각속도 (라디안/초)
float hat_center_transition_timer = 0.0f;
float hat_center_transition_interval = 10.0f;  // 중심이 바뀌는 주기
bool hat_flying = true;

// car sturct
struct Car {
	glm::vec2 position;
	glm::vec2 velocity;
	float scale;
	float scale_delta;
};

Car car1, car2;

void init_car(Car& car, bool left_to_right, bool shirinking) {
	float x, y;

	if (shirinking) {
		y = 150.0f;
	}
	else {
		y = 15.0f;
	}
	x = left_to_right ? -700.0f : 700.0f;

	car.position = glm::vec2(x, y);
	float speed = 0.5f + static_cast<float>(rand() % 15) / 30.0f;
	car.velocity = glm::vec2(left_to_right ? speed : -speed, 0.0f);

	if (shirinking) {
		car.scale = 4.0f;
		car.scale_delta = -0.0025f;
	}
	else {
		car.scale = 0.5f;
		car.scale_delta = 0.005f;
	}
}

void update_cars() {
	car1.scale += car1.scale_delta;
	car2.scale += car2.scale_delta;

	if (car1.scale < 1.0f) car1.scale = 1.0f;
	if (car2.scale > 10.0f) car2.scale = 10.0f;
}

void reset_hat_flight() {
	const float screen_left = -600.0f;
	const float screen_right = 600.0f;
	const float screen_bottom = -400.0f;
	const float screen_top = 400.0f;

	while (true) {
		glm::vec2 new_center = glm::vec2(
			screen_left + rand() % static_cast<int>(screen_right - screen_left),
			screen_bottom + rand() % static_cast<int>(screen_top - screen_bottom)
		);
		if (glm::distance(new_center, hat_cur_center) > 100.0f) {
			hat_prev_center = hat_cur_center;
			hat_nxt_center = new_center;
			break;
		}
	}

	hat_center_transition_timer = 0.0f;
}

// soldier
glm::vec2 soldier_position = glm::vec2(-450.0f, -320.0f); // 초기 위치
glm::vec2 soldier_target_position = glm::vec2(0.0f, 0.0f);
bool soldier_moving = false;
const float soldier_speed = 200.0f;

void update_soldier_position() {
	if (soldier_moving) {
		glm::vec2 direction = soldier_target_position - soldier_position;
		float distance = glm::length(direction);
		glm::vec2 step = glm::normalize(direction) * soldier_speed * delta_time; //한 스텝의 크기

		if (glm::length(step) >= distance) { // 목표 지점과 근사하다면 멈추도록
			soldier_position = soldier_target_position;
			soldier_moving = false;
		}
		else {
			soldier_position += step;
		}
	}
}

//pet
float pet_jump_time = 0.0f;
const float pet_jump_speed = 10.0f; // 강아지 점프 높이
const float pet_jump_frequency = 0.1f; // 강아지 점프 속도


void display(void) {
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float target_brightness = 0.5f + 0.5f * sin(time * 0.3);
	background_brightness += 0.05f * (target_brightness - background_brightness);

	// 녹색 배경 (조도에 따라 어두워짐)
	glm::vec3 bright_bg_color = glm::vec3(0.4f, 0.8f, 0.4f);   // 밝은 녹색
	glm::vec3 dark_bg_color = glm::vec3(0.1f, 0.2f, 0.1f);     // 어두운 녹색
	glm::vec3 current_bg_color = glm::mix(dark_bg_color, bright_bg_color, background_brightness);
	glClearColor(current_bg_color.r, current_bg_color.g, current_bg_color.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -200.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4.0f, 4.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_road();

	car1.position += car1.velocity;
	car2.position += car2.velocity;

	if (car1.position.x > 700.0f || car1.position.x < -700.0f) {
		init_car(car1, rand() % 2 == 0, true);
	}
	if (car2.position.x > 700.0f || car2.position.x < -700.0f) {
		init_car(car2, rand() % 2 == 0, false);
	}
	update_cars();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car1.position, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(car1.scale, car1.scale, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car2.position, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(car2.scale, car2.scale, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	update_soldier_position();

	// soldier_right_part
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(soldier_position, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_soldier();

	glm::mat4 SwordMatrix = glm::translate(ModelMatrix, glm::vec3(45.0f, 10.0f, 1.0f));
	SwordMatrix = glm::rotate(SwordMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	SwordMatrix = glm::scale(SwordMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * SwordMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_sword();

	pet_jump_time += delta_time;
	float jump_offset = sin(pet_jump_time * glm::two_pi<float>() * pet_jump_frequency) * pet_jump_speed;
	glm::mat4 PetMatrix = glm::translate(ModelMatrix, glm::vec3(-80.0f, -40.0f + jump_offset, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * PetMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_pet();

	// soldier_left_part
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(soldier_position, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_soldier();

	// 집 외벽 색상 (기본색 ↔ 밤색)
	glm::vec3 house_bright_color = glm::vec3(235.0f / 255.0f, 225.0f / 255.0f, 196.0f / 255.0f);
	glm::vec3 house_dark_color = glm::vec3(130.0f / 255.0f, 120.0f / 255.0f, 100.0f / 255.0f);
	glm::vec3 current_house_color = glm::mix(house_dark_color, house_bright_color, background_brightness);

	// 창문 색상 (노란빛 ↔ 어두운 회색)
	glm::vec3 window_bright_color = glm::vec3(1.0f, 1.0f, 0.6f); // 불 켜진 창문 (노란빛)
	glm::vec3 window_dark_color = glm::vec3(0.2f, 0.2f, 0.2f);   // 불 꺼진 창문
	glm::vec3 current_window_color = glm::mix(window_dark_color, window_bright_color, 1.0f - background_brightness);

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(450.0f, 350.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_house(current_house_color, current_window_color);


	if (hat_flying) {
		// 중심을 자연스럽게 보간 이동
		hat_center_transition_timer += delta_time;
		float t = glm::clamp(hat_center_transition_timer / hat_center_transition_interval, 0.0f, 1.0f);
		hat_cur_center = glm::mix(hat_prev_center, hat_nxt_center, t);

		// 중심 기준으로 회전하며 위치 계산
		glm::vec2 orbit_offset = glm::vec2(
			hat_cur_radius * cos(hat_cur_angle),
			hat_cur_radius * sin(hat_cur_angle)
		);
		glm::vec2 hat_pos = hat_cur_center + orbit_offset;

		// 회전 각도 증가
		hat_cur_angle += hat_angle_speed * delta_time;

		// 중심 도달 시 재설정
		if (hat_center_transition_timer > hat_center_transition_interval) {
			reset_hat_flight();
		}

		// 변환 적용 (공전 + 자전)
		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(hat_pos, 0.0f));               // 공전 위치로 이동
		ModelMatrix = glm::rotate(ModelMatrix, hat_cur_angle, glm::vec3(0.0f, 0.0f, 1.0f)); // 자전
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.5f, 1.5f, 1.0f));                 // 크기 조절

		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_hat();
	}

	glFlush();
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

// callback 함수 등록
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//윈도우 -> openGL 좌표 변환
		float world_x = static_cast<float>(x) - (win_width / 2.0f);
		float world_y = (win_height / 2.0f) - static_cast<float>(y);

		soldier_target_position = glm::vec2(world_x, world_y);
		soldier_moving = true;
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glutPostRedisplay();
}

void cleanup(void) {
	/*
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
	*/
	// Delete others here too!!!
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.5f, 0.8f, 0.2f, 1.0f); //윈도우 배경 색 지정
	ViewMatrix = glm::mat4(1.0f);
	reset_hat_flight();
	init_car(car1, true, true);
	init_car(car2, false, false);
}

void prepare_scene(void) {
	prepare_road();
	prepare_soldier();
	prepare_house();
	prepare_car();
	prepare_car2();
	prepare_hat();
	prepare_sword();
	prepare_pet();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.0.1.3";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' "
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


