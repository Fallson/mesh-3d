#ifndef CGALQT_COLOR_H
#define CGALQT_COLOR_H

class CColor {
public:
	CColor() {}
	CColor(unsigned char red, 
		unsigned char green, 
		unsigned char blue, 
		unsigned char alpha = 120)
		: _red(red), _green(green), _blue(blue), _alpha(alpha)
	{}

	unsigned char r() const {return _red;}
	unsigned char g() const {return _green;}
	unsigned char b() const {return _blue;}

	unsigned char red() const {return _red;}
	unsigned char green() const {return _green;}
	unsigned char blue() const {return _blue;}
	unsigned char alpha() const {return _alpha;}
	void set_alpha(unsigned char a) {_alpha=a;}
	bool operator==(const CColor &c) const
	{
		return ( (red() == c.red()) &&
			(green() == c.green()) &&
			(blue() == c.blue()) );
	}

	bool operator!=(const CColor &c) const
	{
		return !( (*this) == c);
	}

	CColor& operator=(const CColor &c)
	{
		_red = c.red();
		_green = c.green();
		_blue = c.blue();
		_alpha = c.alpha();
		return *this;
	}

private:
	unsigned char _red;
	unsigned char _green;
	unsigned char _blue;
	unsigned char _alpha;
};

/************************************************************************/
/* color defines                                                         */
/************************************************************************/
#define BGCOLOR CColor(80,245,80)
#define POINTSCOLOR CColor(0,0,0)
#define LINESCOLOR CColor(0,0,0)
#define MESHCOLOR CColor(110,120,138)
#define BBOXCOLOR CColor(110,120,138)
#define SELECTEDCOLOR CColor(255,0,0)
#define FONTCOLOR CColor(0,0,255)

#endif