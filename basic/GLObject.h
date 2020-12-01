#include <list>
#include <functional>

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace GLObj
{
	// GLObj
	//   an interface class
	//   offering methods of transformation, virtual interface of drawing and so on
	//   to create object easily
	class GLObject
	{
	public:
		static std::list<GLObject*> rootObjs; // maintain all NULL-parent objs

		// when you use gluLookUp() or other func to change your camera
		//   the initial mat will not be Indentity
		// we maintain the mat computed from Indentity after camera changed
		//   to avoid push and pop during drawing
		static GLfloat initMat[16];
	private:
		// maintain all child objs
		std::list<GLObject*> children;

		// on parent's coord
		//   for xxxAng[3], idx 0,1,2 are angles about XYZ axis
		GLfloat orbAng[3];
		GLfloat trans[3];
		GLfloat rotAng[3];
		GLfloat scal[3];

		// on world coord
		GLfloat modelMat[16];

		GLObject* parent;

		// an easy way (Lambda expr) to assign animation to this
		std::function<void(GLObject*, long)> AnimeFunc;

		// status flag
		bool isVisible = true;
		bool isAnimated = false;
		bool isModelMatReady = false;
	protected:
		virtual void drawFunc() = 0; // virtual interface of drawing
	public:
		// Constructor
		//   Note: all constructors should be called after openGL initialized
		GLObject(GLObject* parent = NULL) :
			parent(parent)
		{
			// array init
			orbAng[0] = orbAng[1] = orbAng[2] = 0.0f;
			trans[0] = trans[1] = trans[2] = 0.0f;
			rotAng[0] = rotAng[1] = rotAng[2] = 0.0f;
			scal[0] = scal[1] = scal[2] = 1.0f;

			if (!parent) {
				// when parent is null, use identity matrix
				modelMat[1] = modelMat[2] = modelMat[3] =
					modelMat[4] = modelMat[6] = modelMat[7] =
					modelMat[8] = modelMat[9] = modelMat[11] =
					modelMat[12] = modelMat[13] = modelMat[14] = 0.0f;
				modelMat[0] = modelMat[5] = modelMat[10] = modelMat[15] = 1.0f;
			}
			else // when this has parent, use parent's modelMat
				memcpy_s(modelMat, sizeof(float) * 16, parent->modelMat, sizeof(float) * 16);
			isModelMatReady = true;

			if (!parent)
				rootObjs.push_back(this); // when parent is null, this is a root obj, should be added to rootObjs
			else
				parent->children.push_back(this); // when this has parent, should be added to parent's children
		}

		// Destructor
		~GLObject()
		{
			// remove this from higher layer list
			if (!parent)
				rootObjs.remove(this);
			else
				parent->children.remove(this);

			// depth-first-search remove chilren
			std::list<GLObject*>::iterator itr;
			for (itr = children.begin(); itr != children.end(); itr++)
				delete (*itr);
		}

		// flush the transformation
		//   including oribt, translate and rotation
		//   into modelMat
		GLObject& flushTransformation()
		{
			// load initial model mat
			glMatrixMode(GL_MODELVIEW);
			if (!parent)
				glLoadIdentity();
			else
				glLoadMatrixf(parent->modelMat);

			// orbit
			for (int i = 0; i < 3; i++)
				glRotatef(orbAng[i], i == 0 ? 1.0f : 0.0f, i == 1 ? 1.0f : 0.0f, i == 2 ? 1.0f : 0.0f);
			// translate
			glTranslatef(trans[0], trans[1], trans[2]);
			// rotate
			for (int i = 0; i < 3; i++)
				glRotatef(rotAng[i], i == 0 ? 1.0f : 0.0f, i == 1 ? 1.0f : 0.0f, i == 2 ? 1.0f : 0.0f);
			// scale_change
			glScalef(scal[0], scal[1], scal[2]);

			glGetFloatv(GL_MODELVIEW_MATRIX, modelMat); // store the transformation matrix

			// depth-first-search transform chilren
			std::list<GLObject*>::iterator itr;
			for (itr = children.begin(); itr != children.end(); itr++)
				(*itr)->flushTransformation();
			return *this;
		}

		// public draw func
		void draw()
		{
			if (!isVisible)return;

			// load initial model mat to avoid push and pop
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(GLObject::initMat);

			// transform
			glMultMatrixf(modelMat);

			// draw this
			drawFunc();

			// depth-first-search transform chilren
			std::list<GLObject*>::iterator itr;
			for (itr = children.begin(); itr != children.end(); itr++)
				(*itr)->draw();
		}

		// Setter
		//   Note: return *this to apply chain-set
		//     like obj.setOrbit.setTranslate. ...
		GLObject& setParent(GLObject* newParent)
		{
			if (!parent)
				GLObject::rootObjs.remove(this);
			else
				parent->children.remove(this);
			
			if (!newParent)
				GLObject::rootObjs.push_back(this);
			else
				newParent->children.push_back(this);
			return *this;
		}
		GLObject& setAnimeFunc(std::function<void(GLObject*, long)> newAnimeFunc)
		{
			AnimeFunc = newAnimeFunc
			return *this;
		}
		GLObject& setOrbit(GLfloat angx, GLfloat angy, GLfloat angz)
		{
			orbAng[0] = angx;
			orbAng[1] = angy;
			orbAng[2] = angz;
			return *this;
		}
		GLObject& setTranslate(GLfloat tx, GLfloat ty, GLfloat tz)
		{
			trans[0] = tx;
			trans[1] = ty;
			trans[2] = tz;
			return *this;
		}
		GLObject& setRotation(GLfloat angx, GLfloat angy, GLfloat angz)
		{
			rotAng[0] = angx;
			rotAng[1] = angy;
			rotAng[2] = angz;
			return *this;
		}
		GLObject& setScale(GLfloat sx, GLfloat sy, GLfloat sz)
		{
			scal[0] = sx;
			scal[1] = sy;
			scal[2] = sz;
			return *this;
		}
		GLObject* setVisible(bool is) { isVisible = is; }
		GLObject* setAnimated(bool is) { isAnimated = is; }

		// Getter
#define GET_BOOL(val) bool val(){return val;}
		GET_BOOL(isVisible)
			GET_BOOL(isAnimated)
#undef GET_BOOL
	};

	std::list<GLObject*>  GLObject::rootObjs; // static init
}