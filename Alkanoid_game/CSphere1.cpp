#include "CSphere.h"
#include "d3dUtility.h"
#include <stdlib.h> 
#include <cstdlib>
#include <random>


#define M_RADIUS 0.3   // ball radius
#define PI 3.14159265
#define M_HEIGHT 0.1
#define DECREASE_RATE 1 //���� ���� ����
#define Max_Whiteball 5// �� ȭ�鿡 �ִ�� ���� ������ ����� �� (1~5 ���� ����) //
#define Max_life 5 // ���� �� �ִ� ������ �ִ�(1~5���� ����)

// ���� ����, ����, �β� default ��
const float Wall_height = 10.0f;
const float Wall_width = 7.0f;
const float Wall_thickness = 0.2f;

void exitProgram();
extern int lifeCnt; //���� ��� ���� counting
extern int ballCnt; //�� ���� counting

// ��ü ����Ʈ
extern int total_point;

std::random_device r;
std::mt19937 c(r());


CSphere::CSphere(void)
{
	D3DXMatrixIdentity(&m_mLocal);
	ZeroMemory(&m_mtrl, sizeof(m_mtrl));
	m_radius = 0;
	m_velocity_x = 0;
	m_velocity_z = 0;
	m_pSphereMesh = NULL;
}



void CSphere::set_ignore_collision(bool ignore) {
	ignore_collision = ignore;
}

// inline ����
bool CSphere::get_ignore_collision() {
	return ignore_collision;
}

void CSphere::set_item(bool has_item) {
	this->has_item = has_item;
}

bool CSphere::is_has_item() {
	return has_item;
}

int CSphere::get_nochangetime() { // no_change_time ���
	return no_change_time;
}

void CSphere::set_nochangetime(int time) { // no_change_time �� ����
	no_change_time = time;
}

float CSphere::get_centerx() { //center_x ���
	return this->center_x;
}

float CSphere::get_centery() { //center_y ���
	return this->center_y;
}

float CSphere::get_centerz() { //center_z ���
	return this->center_z;
}

void  CSphere::set_exist(bool sphere_exist) { //���� ȭ�鿡 �����ϴ� �� ����
	this->sphere_exist = sphere_exist;
}

bool CSphere::get_exist() { // ���� �����ϴ� �� ������ ��� �Լ�
	return this->sphere_exist;
}

void CSphere::set_whiteball(bool white_ball) { // �߻�Ǵ� �� or �����̴� �� ���� ���� (�߻�Ǵ� ���� �浹�� ������� �ȵ�)
	this->white_ball = white_ball;
}

// inline ����
bool CSphere::get_whiteball() { // ���(�߻�Ǵ� ��)�� �´� �� Ȯ���ϴ� ����
	return this->white_ball;
}

bool CSphere::is_balloutside(bool& bug) { // ���� ������ ��� ������ ������ ��?
	if (center_z < (-1) * Wall_height / 2 - 0.01) { //�Ʒ��� ������ ���
		ballCnt--;
		return true;
	}

	if (center_x < (-1) * Wall_width / 2 - 0.01 || center_x> Wall_width / 2 + 0.01 || center_z > Wall_height / 2 + 0.01) { //���׷� �߸��� ��ġ�� ���� ���
		bug = true; // �������� �� Ż��(����)
		return true;
	}
	return false;
}

/*void CSphere::ball_position() { // ���� �ùٸ� ��ġ�� �ƴ϶�� ��ġ�� �����ϴ� �Լ�. (�Ⱦ��� �Լ�)
		if (is_balloutside()) {
			setPower(0, 0);
			center_x = spherePos[0][0];
			center_z = spherePos[0][1];

		}
	}
*/

bool CSphere::create(IDirect3DDevice9* pDevice, D3DXCOLOR color) // Default�� �������� �����鼭 ���ҽ� ������ ���� ����.
{
	if (NULL == pDevice)
		return false;

	m_mtrl.Ambient = color;
	m_mtrl.Diffuse = color;
	m_mtrl.Specular = color;
	m_mtrl.Emissive = d3d::BLACK;
	m_mtrl.Power = 5.0f;
	m_radius = M_RADIUS; // ������ �ٸ� �� ����� ���� 

	if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &m_pSphereMesh, NULL)))
		return false;
	return true;
}

void CSphere::destroy(void)
{
	if (m_pSphereMesh != NULL) {
		m_pSphereMesh->Release();
		m_pSphereMesh = NULL;
	}
}

void CSphere::draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
{
	if (NULL == pDevice)
		return;
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
	pDevice->SetMaterial(&m_mtrl);
	m_pSphereMesh->DrawSubset(0);
}

// inline ����
bool CSphere::hasIntersected(CSphere& ball)
{
	if (ball.get_nochangetime() > 0) { // �浹���� �ʹ� ���� �ð��� ���
		return false; //�浹 ���ߴٰ� �Ǵ�
	}

	float distance_square = pow((ball.center_x - this->center_x), 2) + pow(ball.center_z - this->center_z, 2); // �� ���� ���� ������ �Ÿ��� ����
	float distance = abs(pow(distance_square, 0.5)); // �� �� ���� ������ �Ÿ�
	if (distance <= (ball.getRadius() + this->getRadius()) + 0.1) { // �� ���� �������� �պ��� �Ÿ��� ������ �浹���� 
		return true;
	}
	else {
		return false;
	}

}

void CSphere::hitBy(CSphere& ball)
{

	if (hasIntersected(ball)) { // �浹�� �߻� �Ҷ�
		ball.set_nochangetime(2); // 2�� ���� ������ ��ȭ ����


		// ���ϴ� ������ ������ ���� �Ǽ� ����
		std::uniform_real_distribution<double> distribution(-1.0, 1.0);
		double random_number1 = distribution(c);
		double random_number2 = distribution(c);

		float speedup1 = 0;
		float speedup2 = 0;
		float speedup3 = 0;
		float speedup4 = 0;

		if (abs(-ball.getVelocity_X() + random_number1) < 2) { // �ʹ� �������� �� ������ �ӵ� ����
			if (-ball.getVelocity_X() + random_number1 >= 0) {
				speedup1 = 2;
			}
			else {
				speedup1 = -2;
			}
		}

		if (abs(-ball.getVelocity_Z() + random_number2) < 2) {
			if (-ball.getVelocity_Z() + random_number2 >= 0) {
				speedup2 = 2;
			}
			else {
				speedup2 = -2;
			}
		}

		if (abs(getVelocity_X() + random_number1) < 2) {
			if (getVelocity_X() + random_number1 >= 0) {
				speedup3 = 2;
			}
			else {
				speedup3 = -2;
			}
		}

		if (abs(getVelocity_Z() + random_number2) < 2) {
			if (getVelocity_Z() + random_number2 >= 0) {
				speedup4 = 2;
			}
			else {
				speedup4 = -2;
			}
		}

		if (!ball.get_whiteball()) {
			ball.set_exist(false); // ���� ȭ�鿡�� ���ش�.
		}
		else {
			if (!ball.ignore_collision && !ignore_collision) {
				ball.setPower(-ball.getVelocity_X() + random_number1 + speedup1, -ball.getVelocity_Z() + random_number2 + speedup2); // �� �������� ���̰� �浹 ����޴� ���¸� �ӵ��� ������Ʈ �Ѵ�.

			}

		}
		if (!this->get_whiteball()) {
			this->set_exist(false); // ���� ȭ�鿡�� ���ش�.
		}
		else {
			if (!ball.ignore_collision && !ignore_collision) {
				this->setPower(-getVelocity_X() + random_number1 + speedup3, -getVelocity_Z() + random_number2 + speedup4);  // �� �������� ���̰� �浹 ����޴� ���¸�  �ӵ��� ������Ʈ �Ѵ�.
			}

		}

		// ����� ������ �浹�ϸ� �߰� ÷��
		if (this->sphere_exist) plusPoint();

		// block�� item������ �ִ� ������ Ȯ���ϱ�
		if (ball.is_has_item()) {
			this->has_item = true;
			ball.set_item(false); // �������� �Ծ����Ƿ� ���� item�� false ó�� ���ش�.
		}
	}

	// Insert your code here.
}

void CSphere::ballUpdate(float timeDiff)
{
	const float TIME_SCALE = 3.3;
	D3DXVECTOR3 cord = this->getCenter();
	double vx = abs(this->getVelocity_X());
	double vz = abs(this->getVelocity_Z());

	if (vx > 0.01 || vz > 0.01)
	{
		float tX = cord.x + TIME_SCALE * timeDiff * m_velocity_x;
		float tZ = cord.z + TIME_SCALE * timeDiff * m_velocity_z;

		//correction of position of ball
		// Please uncomment this part because this correction of ball position is necessary when a ball collides with a wall
		/*if(tX >= (4.5 - M_RADIUS))
			tX = 4.5 - M_RADIUS;
		else if(tX <=(-4.5 + M_RADIUS))
			tX = -4.5 + M_RADIUS;
		else if(tZ <= (-3 + M_RADIUS))
			tZ = -3 + M_RADIUS;
		else if(tZ >= (3 - M_RADIUS))
			tZ = 3 - M_RADIUS;*/

		this->setCenter(tX, cord.y, tZ);
	}
	else { this->setPower(0, 0); }
	//this->setPower(this->getVelocity_X() * DECREASE_RATE, this->getVelocity_Z() * DECREASE_RATE);
	double rate = 1 - (1 - DECREASE_RATE) * timeDiff * 400;
	if (rate < 0)
		rate = 0;
	this->setPower(getVelocity_X() * rate, getVelocity_Z() * rate);
}

// inline ����
double CSphere::getVelocity_X() { return this->m_velocity_x; }
double CSphere::getVelocity_Z() { return this->m_velocity_z; }

void CSphere::setPower(double vx, double vz)
{
	if (vx > 4) { // �ִ� �ӵ� ����
		this->m_velocity_x = 4;
	}
	else if (vx < -4) {
		this->m_velocity_x = -4;
	}
	else {
		this->m_velocity_x = vx;
	}

	if (vz > 4) { // �ִ� �ӵ� ����
		this->m_velocity_z = 4;
	}
	else if (vz < -4) {
		this->m_velocity_z = -4;
	}
	else {
		this->m_velocity_z = vz;
	}

}

void CSphere::setCenter(float x, float y, float z)
{
	D3DXMATRIX m;
	center_x = x;	center_y = y;	center_z = z;
	m_radius = y;
	D3DXMatrixTranslation(&m, x, y, z);
	setLocalTransform(m);
}

float CSphere::getRadius(void)  const { return (float)(m_radius); } // ������ �ٸ��� ���� �����ϹǷ� ���������� ����
const D3DXMATRIX& CSphere::getLocalTransform(void) const { return m_mLocal; }
void CSphere::setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
D3DXVECTOR3 CSphere::getCenter(void) const
{
	D3DXVECTOR3 org(center_x, center_y, center_z);
	return org;
}

void CSphere::plusPoint()
{
	if (this->first_hit) {
		this->first_hit = false;
		this->point = 10;
		total_point += this->point;
	}
	else {
		this->point = this->point * 2;
		if (point == 160) this->point = this->point / 2;
		total_point += this->point;
	}
}

void CSphere::setPoint(int point) {
	this->point = point;
}

void CSphere::setFirstHit(boolean first_hit) {
	this->first_hit = first_hit;
}

void CSphere::setActive(boolean active) {
	this->active = active;
}

bool CSphere::getActive() {
	return this->active;
}