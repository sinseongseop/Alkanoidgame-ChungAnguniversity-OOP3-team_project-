#ifndef CSphere_H
#define CSphere_H
#define M_RADIUS 0.21   // ball radius


#include "d3dUtility.h"
#include <vector>

class CSphere {
private:
	int						no_change_time = 0; // �̰��� ����� ��� �浹�� �����Ѵ�.
	float					center_x, center_y, center_z;
	float                   m_radius;
	float					m_velocity_x;
	float					m_velocity_z;
	bool					sphere_exist = true; // ���� ������ �����ϴ� ��. �浹�� False�� ����
	bool					white_ball = false; //�߻�Ǵ� �� or �����̴� �� �ΰ�? (�浹 �ÿ��� ��� �����ؾ���)
	bool					has_item = false; // �� ���� ���� �������� ������ ��?
	bool					ignore_collision = false;// ������ �浹 �Ҷ� ���� ���� ������ �ٲ�°�?
	int						point; // ����� �������� �ε����� ���� �߰��Ǵ� ���� (�ٿ��� ƨ�� �� ó������ Ȯ�����־����)
	bool					first_hit = true; // �ٿ��� ƨ�� �� ó�� �ε������� Ȯ��
	bool					active = true; // ���� �����̴� �ǿ� ƨ��� �� ��Ŭ������ ������ ������ �� �ִ� ���� (���� ƨ�涧�� ��Ŭ������ �����̴� �� ����)


public:
	CSphere(void);
	~CSphere(void) {};

public:
	void set_ignore_collision(bool ignore);

	//inline ����
	bool get_ignore_collision();

	void set_item(bool has_item);

	bool is_has_item();

	int get_nochangetime(); // no_change_time ���	

	void set_nochangetime(int); // no_change_time �� ����


	float get_centerx(); //center_x ���

	float get_centery(); //center_y ���

	float get_centerz(); //center_z ���

	void set_exist(bool); //���� ȭ�鿡 �����ϴ� �� ����

	bool get_exist(); // ���� �����ϴ� �� ������ ��� �Լ�


	void set_whiteball(bool); // �߻�Ǵ� �� or �����̴� �� ���� ���� (�߻�Ǵ� ���� �浹�� ������� �ȵ�)

	//inline ����
	bool get_whiteball(); // ���(�߻�Ǵ� ��)�� �´� �� Ȯ���ϴ� ����


	bool is_balloutside(bool&);// ���� ������ ��� ������ ������ ��?

	//void ball_position(); // ���� �ùٸ� ��ġ�� �ƴ϶�� ��ġ�� �����ϴ� �Լ�.



	bool create(IDirect3DDevice9* pDevice, D3DXCOLOR color = d3d::WHITE);// Default�� �������� �����鼭 ���ҽ� ������ ���� ����.


	void destroy(void);

	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld);

	bool hasIntersected(CSphere& ball);

	void hitBy(CSphere& ball);

	void ballUpdate(float timeDiff);

	// inline ����
	double getVelocity_X();
	double getVelocity_Z();

	void setPower(double vx, double vz);


	void setCenter(float x, float y, float z);

	float getRadius(void)  const; // ������ �ٸ��� ���� �����ϹǷ� ���������� ����
	const D3DXMATRIX& getLocalTransform(void) const;
	void setLocalTransform(const D3DXMATRIX& mLocal);
	D3DXVECTOR3 getCenter(void) const;

	// �߰� ���� �Լ�
	void plusPoint();
	void setPoint(int point);
	void setFirstHit(boolean first_hit);

	// ��Ŭ�� ������ ���� �Լ�
	void setActive(boolean active);
	bool getActive();

private:
	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pSphereMesh;
};




#endif