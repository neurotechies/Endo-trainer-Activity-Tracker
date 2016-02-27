//overall input-> Quaternion, sensor acceleration
// output-> euler angles, yaw pitch roll, gravity, linear acceleration, linear acceleration in world frame



//1. Quaternion to euler
//input -> Quaternion (four floats-> w,x,y,z)
//output -> euler     (three floats -> psi, theta , phi) // angles in radians // multiply (180/pi) to get angle in degree

uint8_t MPU6050::dmpGetEuler(float *data, Quaternion *q) 
{
    data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);   // psi
    data[1] = -asin(2*q -> x*q -> z + 2*q -> w*q -> y);                              // theta
    data[2] = atan2(2*q -> y*q -> z - 2*q -> w*q -> x, 2*q -> w*q -> w + 2*q -> z*q -> z - 1);   // phi
    return 0;
}

//2. Quaternion to gravity
//input -> Quaternion (four floats-> w,x,y,z)
//output -> gravity   (three floats -> x,y,z)

uint8_t MPU6050::dmpGetGravity(VectorFloat *v, Quaternion *q) {
    v -> x = 2 * (q -> x*q -> z - q -> w*q -> y);
    v -> y = 2 * (q -> w*q -> x + q -> y*q -> z);
    v -> z = q -> w*q -> w - q -> x*q -> x - q -> y*q -> y + q -> z*q -> z;
    return 0;
}

//3. Quaternion to yaw pitch roll
//input -> Quaternion (four floats-> w,x,y,z), gravity (three floats, x,xy,z)
//output -> gravity   (three floats -> yaw, pitch roll) // angles in radians // multiply (180/pi) to get angle in degree

uint8_t MPU6050::dmpGetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity) {
    // yaw: (about Z axis)
    data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);
    // pitch: (nose up/down, about Y axis)
    data[1] = atan(gravity -> x / sqrt(gravity -> y*gravity -> y + gravity -> z*gravity -> z));
    // roll: (tilt left/right, about X axis)
    data[2] = atan(gravity -> y / sqrt(gravity -> x*gravity -> x + gravity -> z*gravity -> z));
    return 0;
}

//4. Linear acceleration from sensor acceleration and gravity
//input(vRaw, gravity) -> sensor acceleration(three 1nt16-> x,y,z), gravity (three floats, x,y,z)
//output(v) -> linear acceleration   (three floats -> yaw, pitch roll) 

uint8_t MPU6050::dmpGetLinearAccel(VectorInt16 *v, VectorInt16 *vRaw, VectorFloat *gravity) {
    // get rid of the gravity component (+1g = +8192 in standard DMP FIFO packet, sensitivity is 2g)
    v -> x = vRaw -> x - gravity -> x*8192;
    v -> y = vRaw -> y - gravity -> y*8192;
    v -> z = vRaw -> z - gravity -> z*8192;
    return 0;
}

//5. linear acceleration in world frame.
//input(vReal, q) -> linear acceleration(dmpGetLinearAccel) (three int16-> x,y,z), Quaternion (four floats-> w,x,y,z)
//output(v) -> linear acceleration in world frame  (three int_16 -> x,y,z)

uint8_t MPU6050::dmpGetLinearAccelInWorld(VectorInt16 *v, VectorInt16 *vReal, Quaternion *q) {
    // rotate measured 3D acceleration vector into original state
    // frame of reference based on orientation quaternion
    memcpy(v, vReal, sizeof(VectorInt16));
	
	// P_out = q * P_in * conj(q)
	// - P_out is the output vector
	// - q is the orientation quaternion
	// - P_in is the input vector (a*aReal)
	// - conj(q) is the conjugate of the orientation quaternion (q=[w,x,y,z], q*=[w,-x,-y,-z])

	Quaternion p(0, v->x, v->y, v->z);

	// quaternion multiplication: q * p, stored back in p
	p = q -> getProduct(p);

	// quaternion multiplication: p * conj(q), stored back in p
	p = p.getProduct(q -> getConjugate());

	// p quaternion is now [0, x', y', z']
	v->x = p.x;
	v->y = p.y;
	v->z = p.z;
    return 0;
}

// Quaternion class

class Quaternion {
    public:
        float w;
        float x;
        float y;
        float z;
        
        Quaternion() {
            w = 1.0f;
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }
        
        Quaternion(float nw, float nx, float ny, float nz) {
            w = nw;
            x = nx;
            y = ny;
            z = nz;
        }

        Quaternion getProduct(Quaternion q) {
            // Quaternion multiplication is defined by:
            //     (Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
            //     (Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
            //     (Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
            //     (Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2
            return Quaternion(
                w*q.w - x*q.x - y*q.y - z*q.z,  // new w
                w*q.x + x*q.w + y*q.z - z*q.y,  // new x
                w*q.y - x*q.z + y*q.w + z*q.x,  // new y
                w*q.z + x*q.y - y*q.x + z*q.w); // new z
        }

        Quaternion getConjugate() {
            return Quaternion(w, -x, -y, -z);
        }
        
        float getMagnitude() {
            return sqrt(w*w + x*x + y*y + z*z);
        }
        
        void normalize() {
            float m = getMagnitude();
            w /= m;
            x /= m;
            y /= m;
            z /= m;
        }
        
        Quaternion getNormalized() {
            Quaternion r(w, x, y, z);
            r.normalize();
            return r;
        }
};