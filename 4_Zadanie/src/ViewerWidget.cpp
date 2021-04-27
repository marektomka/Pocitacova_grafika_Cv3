#include   "ViewerWidget.h"
#include <math.h> 

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
	body.clear();
	ebody.clear();
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) {
		data[x + y * img->width()] = color.rgb();
	}
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, color);
		}
	}
	update();
}


void ViewerWidget::drawDDA(int x1, int y1, int x2, int y2, QColor col)
{	

	double m = DBL_MAX;      // smernica
	int X, Y;               // pomocne pre vymenu suradnic
	double xo, yo;			// pomocne pre posunutie v danom smere
	
	m = (y2 - y1) / (double)(x2 - x1);
	m = abs(m);

	if (m <= 1)
	{
		if (x1 > x2)
		{
			X = x2; x2 = x1; x1 = X;
			Y = y2; y2 = y1; y1 = Y;
		}
		yo = y1;
		setPixel(x1, y1, col);

		do
		{
			x1 = x1 + 1;
			if (y1 < y2) {
				yo = yo + m;
			}
			else {
				yo = yo - m;
			}
			setPixel(x1, static_cast<int>(yo + 0.5), col);
		} while (x1 < x2);

	}
	else if (m >= 1)
	{
		if (y1 > y2)
		{
			Y = y2; y2 = y1; y1 = Y;
			X = x2; x2 = x1; x1 = X;

		}
		xo = x1;
		setPixel(x1, y1, col);

		do
		{
			y1 = y1 + 1;
			if (x1 < x2) {
				xo = xo + (1 / m);
			}
			else {
				xo = xo - (1 / m);
			}
			setPixel(static_cast<int>(xo + 0.5), y1, col);
		} while (y1 < y2);
	}
	update();
}

void ViewerWidget::drawBresen(int x1, int y1, int x2, int y2, QColor col)
{
	int k1, k2, p1;
	double m = DBL_MAX;
	int X, Y;

	m = (y2 - y1) / (double)(x2 - x1);

	if (m > 0 && m < 1) 
	{
		if (x1 > x2) 
		{
			X = x2; x2 = x1; x1 = X;
			Y = y2; y2 = y1; y1 = Y;
		}
		k1 = 2 * (y2 - y1);
		k2 = k1 - 2 * (x2 - x1);
		p1 = k1 - (x2 - x1);
		setPixel(x1, y1, col);

		do
		{
			x1 += 1;
			if (p1 > 0) {
				p1 = p1 + k2;
				if (y1 < y2) {
					y1 += 1;
				}
				else {
				y1 -= 1;
				}
			}
			else {
				p1 = p1 + k1;
			}
			setPixel(x1, y1, col);

		} while (x1 < x2);
	}
	else if (m > -1 && m <= 0) 
	{
		if (x1 > x2)
		{
			X = x2; x2 = x1; x1 = X;
			Y = y2; y2 = y1; y1 = Y;
		}
		k1 = 2 * (y2 - y1);
		k2 = k1 + 2 * (x2 - x1);
		p1 = k1 + (x2 - x1);
		setPixel(x1, y1, col);

		do
		{
			x1 += 1;
			if (p1 < 0) {
				y1 -= 1;
				p1 = p1 + k2;
			}
			else {
				p1 = p1 + k1;
			}
			setPixel(x1, y1, col);

		} while (x1 < x2);

	}
	else if (m >= 1) 
	{
		if (y1 > y2)
		{
			X = x2; x2 = x1; x1 = X;
			Y = y2; y2 = y1; y1 = Y;
		}
		k1 = 2 * (x2 - x1);
		k2 = k1 - 2 * (y2 - y1);
		p1 = k1 - (y2 - y1);
		setPixel(x1, y1, col);

		do
		{
			y1 += 1;
			if (p1 > 0) {
				p1 = p1 + k2;
				if (y1 < y2) {
					x1 += 1;
				}
				else {
					x1 -= 1;
				}
			}
			else {
				p1 = p1 + k1;
			}
			setPixel(x1, y1, col);
		} while (y1 < y2);

	}
	else if (m <= -1)
	{
		if (y1 > y2)
		{
			X = x2; x2 = x1; x1 = X;
			Y = y2; y2 = y1; y1 = Y;
		}

		k1 = 2 * (x2 - x1);
		k2 = k1 + 2 * (y2 - y1);
		p1 = k1 + (y2 - y1);
		setPixel(x1, y1, col);

		do
		{
			y1 += 1;
			if (p1 < 0) {
				x1 -= 1;
				p1 = p1 + k2;
			}
			else {
				p1 = p1 + k1;
			}
			setPixel(x1, y1, col);

		} while (y1 < y2);

	}
	update();
}

void ViewerWidget::drawBresenCircle(int x1, int y1, int x2, int y2, QColor col)
{
	int r = 0;
	int p1;
	int X, Y,dvaX,dvaY;
	r = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	p1 = 1 - r;
	X = 0;
	Y = r ;
	dvaX = 3;
	dvaY = 2 * r + 2;
	do
	{
		setPixel(X+x1, Y+y1, col);
		setPixel(-X + x1, Y + y1, col);
		setPixel(X + x1, -Y + y1, col);
		setPixel(-X + x1, -Y + y1, col);
		setPixel(Y + x1,X + y1, col);
		setPixel(-Y + x1, X + y1, col);
		setPixel(Y + x1, -X + y1, col);
		setPixel(-Y + x1, -X + y1, col);
		if (p1 > 0) {
			p1 = p1 - dvaY;
			Y -= 1;
			dvaY -= 2;
		}
		p1 = p1 + dvaX;
		dvaX += 2;
		X += 1;
		

	} while (X<=Y);

	update();
}

bool ViewerWidget::CyrusBeck()
{	
	newbody.clear();
	setEbody();
	bool out = true;
	for (size_t i = 0; i < 4; i++)
	{
		QPoint n = QPoint(-(ebody[i].y() - ebody[i + 1].y()), ebody[i].x() - ebody[i + 1].x());
		QPoint pMe1 = body[0] - ebody[i];
		QPoint pMe2 = body[1] - ebody[i];

		int nXp1 = (n.x() * pMe1.x() + n.y() * pMe1.y());
		int nXp2 = (n.x() * pMe2.x() + n.y() * pMe2.y());
		if (nXp1 > 0 || nXp2 > 0){ out = true;}
		else { out = false; break; }
	}

	
	if (out) return false;
	else {
		QPoint d;
		double tl = 0, tu = 1;
		d = body[1] - body[0];
		for (size_t i = 0; i < 4; i++)
		{

			QPoint n = QPoint(-(ebody[i + 1].y() - ebody[i].y()), ebody[i + 1].x() - ebody[i].x());
			QPoint w = body[0] - ebody[i];


			int dn = d.x() * n.x() + d.y() * n.y();
			int wn = w.x() * n.x() + w.y() * n.y();
			if (dn != 0) {
				double t = -wn / static_cast<double>(dn);

				if (dn > 0 && t <= 1) {
					if (t > tl) {	// max
						tl = t;
					}
				}
				if (dn < 0 && t >= 0) {
					if (t < tu) {   // min
						tu = t;
					}
				}
			}
		}
		if (tl == 0 && tu == 1) {
			newbody.push_back(QPoint(body[0]));
			newbody.push_back(QPoint(body[1]));
			
		}
		else if (tl < tu) {
			newbody.push_back(body[0] + QPoint(d * tl));
			newbody.push_back(body[0] + QPoint(d * tu));
		
		}
		return true;

	}


}

void ViewerWidget::Sutherland()
{
	newbody.clear();
	QVector<QPoint> V = body;
	QVector<QPoint> W;
	int minX[4]{ 0,0,(-1)*(img->width() - 1),(-1)*(img->height() - 1)};

	if (V.size() > 2) {

		for (int i = 0; i < 4; i++) {
			QPoint S = V[V.size() - 1];
			int Xmin = minX[i];

			for  (int j = 0; j < V.size(); j++){
				if (V[j].x() >= Xmin) {
					if (S.x() >= Xmin){
						W.append(V[j]);
					}
					else {
						QPoint P(Xmin, static_cast<int>(S.y() + (Xmin - S.x()) * ((V[j].y() - S.y()) / static_cast<double>(V[j].x() - S.x()))));
						W.append(P);
						W.append(V[j]);
					}
				}
				else 
				{
					if (S.x() >= Xmin) {
						QPoint P(Xmin, static_cast<int>(S.y() + (Xmin - S.x()) * ((V[j].y() - S.y()) / static_cast<double>(V[j].x() - S.x()))));
						W.append(P);
					}
				}
				S = V[j];
			}
			V = W;
			W.clear();

			for (int j = 0; j < V.size(); j++) {
				V[j] = QPoint(V[j].y(), V[j].x() * (-1));
			}
		}
		newbody = V;	
	}
}

void ViewerWidget::setEbody()
{
	ebody.clear();
	ebody.push_back(QPoint(0, 0));	
	ebody.push_back(QPoint(getImgWidth()-1, 0));
	ebody.push_back(QPoint(getImgWidth()-1, getImgHeight()-1));
	ebody.push_back(QPoint(0, getImgHeight()-1));
	ebody.push_back(QPoint(0, 0));

	
}

void ViewerWidget::clearPush()
{
	body.clear();
	newbody.clear();
	ipolT.clear();
	button = FALSE;
	interpol = 0;
	ipolchecked = FALSE;
	fillColor = Qt::black;
}

void ViewerWidget::drawPolyg(QColor col, int method)
{
	clear();
	meth = method;
	
	if (body.size() == 2)
	{
		CyrusBeck();
		if (method == 0) {
			drawDDA(newbody[0].x(), newbody[0].y(), newbody[1].x(), newbody[1].y(), col);
		}
		else if (method == 1) {
			drawBresen(newbody[0].x(), newbody[0].y(), newbody[1].x(), newbody[1].y(), col);
		}
	}
	else if (body.size() > 2) {
		Sutherland();
		for (size_t i = 0; i < newbody.size()-1; i++)   
		{
			if (method == 0) {
				drawDDA(newbody[i].x(),newbody[i].y(), newbody[i + 1].x(), newbody[i + 1].y(), col);
			}else if (method == 1) {
				drawBresen(newbody[i].x(), newbody[i].y(), newbody[i + 1].x(), newbody[i + 1].y(), col);
			}	
		}
		if (button == TRUE) {
			if (method == 0) {
				drawDDA(newbody[newbody.size() - 1].x(), newbody[newbody.size() - 1].y(), newbody[0].x(), newbody[0].y(), col);
			}else {
				drawBresen(newbody[newbody.size() - 1].x(), newbody[newbody.size() - 1].y(), newbody[0].x(), newbody[0].y(), col);
			}
		}
	}
}

void ViewerWidget::setPoints(QPoint bod)
{
	body.push_back(bod);
}

void ViewerWidget::setChPoints(QPoint bod, int index)
{
	body[index] = bod;
}

void ViewerWidget::rightbutton()
{
	button =  TRUE;
}

void ViewerWidget::ScanLine()
{
	
	//--------------------------------PREDPRIPRAVA HRAN--------------------------------------------------//
	struct edge {
		int xz, yz;
		int xk,yk;
		double m;
	};

	QVector<edge> edges;							// všetky pôvodné hrany
	newbody.append(newbody[0]);						// uzavretie polygonu
	for (size_t i = 0; i < newbody.size()-1; i++)
	{
		edge e{};									// len pomocne na zoradenie hran
		if (newbody[i].y() < newbody[i+1].y())      // zoradenie podla yz < yk
		{
			e.m = (newbody[i + 1].y() - newbody[i].y()) * 1.0 / (newbody[i + 1].x() - newbody[i].x());
			e.xz = newbody[i].x();
			e.yz = newbody[i].y();
			e.xk = newbody[i + 1].x();
			e.yk = newbody[i + 1].y()-1;
		}
		else {
			e.m = (newbody[i].y() - newbody[i+1].y()) * 1.0 / (newbody[i].x() - newbody[i + 1].x());
			e.xz = newbody[i+1].x();
			e.yz = newbody[i+1].y();
			e.xk = newbody[i].x();
			e.yk = newbody[i].y()-1;
			
		}
		edges.append(e);
	}
	newbody.removeLast();							// odstranenie bodu, ktory som pridaval hore 

	for (size_t i = 0; i < edges.size() -1 ; i++){  // zoradenie vzostupne podla yz
		for (size_t j = 0; j < edges.size() - i - 1; j++){
			if (edges[j].yz > edges[j+1].yz){
				edge exch;
				exch = edges[j];
				edges[j] = edges[j + 1];
				edges[j + 1] = exch;
			}
		}
	}

	/*for (size_t i = 0; i < edges.size(); i++)
	{
		edges[i].m = (edges[i].yk - edges[i].yz) / (edges[i].xk - edges[i].xz) ;
	}
	
	int YMAX = edges[0].yk;
	for (size_t i = 0; i < edges.size() -1; i++)
	{
		if (edges[i].yk < edges[i + 1].yk) {
			YMAX = edges[i + 1].yk;
		}
	}
	
	int ymax = YMAX;*/

	int ymin = edges[0].yz;
	int ymax = edges[edges.size() - 1].yk;
	

	//--------------------------------------------TABULKY HRAN-------------------------------------------//
	
	struct scanlineEdge {
		int dy;										// poèet riadkov do ktorých hrana zasahuje
		double x;									// aktuálna súradnica s rozkladovým riadkom
		double w;									// prírstok x pri prechode na ïa¾ší riadok
	};

	QVector<QList<scanlineEdge>> edgesTable;       // TH 
	edgesTable.resize(ymax - ymin + 1);
	QVector<scanlineEdge> sc;
	for (size_t i = 0; i < edges.size(); i++)
	{
		scanlineEdge scedge;
		scedge.dy = edges[i].yk - edges[i].yz;
		scedge.x = edges[i].xz * 1.0;
		scedge.w = 1 / edges[i].m;
		sc.append(scedge);
	}
	for (size_t i = 0; i < edgesTable.size()-1; i++) {
		for (size_t j = 0; j < sc.size(); j++)
		{
			if (edges[j].yz - ymin == i) {
				edgesTable[i].append(sc[j]);
			}
		}
	}


	QVector<scanlineEdge> activeEdges;
	
	int y = ymin;

	for (size_t i = 0; i < edgesTable.size(); i++)
	{
			for (size_t j = 0; j < edgesTable[i].size(); j++)               //  presunutie hran z TH
			{
				if (edgesTable[i].size() != 0) {
				activeEdges.append(edgesTable[i][j]);
				}
			}

		for (size_t ij = 0; ij < activeEdges.size() - 1; ij++)
		{
			for (size_t j = 0; j < activeEdges.size() - ij - 1; j++)		// zoradenie ZAH vzostupne
			{
				if (activeEdges[j].x > activeEdges[j + 1].x) {
					scanlineEdge exch;
					exch = activeEdges[j];
					activeEdges[j] = activeEdges[j + 1];
					activeEdges[j + 1] = exch;
				}
			}
		}
		for (size_t j = 0; j < activeEdges.size() - 1; j+=2)				// po dvojiciach prechadzame cez ZAH a malujeme
		{
			if (activeEdges[j].x != activeEdges[j+1].x){
				for (size_t pixel = int(activeEdges[j].x + 0.5) ; pixel < int(activeEdges[j + 1].x + 0.5); pixel++)
				{
					setPixel(pixel, y , fillColor);
					update();
				}
			}
		}

		/*for (size_t j = 0; j < activeEdges.size(); j++)						// odstranenie vsetkych hran s dy == 0;
		{
			if (activeEdges[j].dy == 0){
				activeEdges.remove(j);
			}
		}*/
		for (size_t ii = 0; ii < activeEdges.size(); ii++)
		{
			for (size_t j = 0; j < activeEdges.size(); j++)
			{
				if (activeEdges[j].dy == 0) {
					activeEdges.remove(j);
				}
			}
		}

		for (size_t j = 0; j < activeEdges.size(); j++)						// aktualizujeme hodnoty ZAH
		{
			activeEdges[j].dy = activeEdges[j].dy - 1;
			activeEdges[j].x = activeEdges[j].x + activeEdges[j].w *1.0;
		}
		y++;
	}

}

void ViewerWidget::ScanLineTriangle()
{
	QVector<QPoint> T;

	T = newbody;

	for (size_t i = 0; i < 3; i++)								// utriedenie podla y
	{
		for (size_t j = 0; j < 2; j++)
		{
			if (T[j].y() > T[j + 1].y())
			{
				QPoint exch = T[j];
				T[j] = T[j + 1];
				T[j + 1] = exch;
			}
			else if (T[j].y() == T[j + 1].y()) {					// utriedenie podla x
				if (T[j].x() > T[j + 1].x()){
					QPoint exch = T[j];
					T[j] = T[j + 1];
					T[j + 1] = exch;
				}
			}
		}
	}
	ipolT = T;                                         // pomocne pre interpolaciu
	if (T[0].y() == T[1].y())
	{
		ScanLineTriangleDown(T[0], T[1], T[2]);
	}
	else if (T[1].y() == T[2].y()) {
		ScanLineTriangleUp(T[0], T[1], T[2]);
	}
	else {
		QPoint P;
		double m;
		m = (T[2].y() - T[0].y()) / (T[2].x() - T[0].x()*1.0);
		P.setX(int((T[1].y() - T[0].y()) /m + T[0].x()));
		P.setY(T[1].y());
		if (T[1].x() < P.x()) {
			ScanLineTriangleUp(T[0], T[1], P);
			ScanLineTriangleDown(T[1], P, T[2]);
		}
		else{
			ScanLineTriangleUp(T[0], P, T[1]);
			ScanLineTriangleDown(P, T[1], T[2]);
		}
	}

	update();
}

void ViewerWidget::ScanLineTriangleDown(QPoint T0, QPoint T1, QPoint T2)
{
	struct edge {
		QPoint up, down;
		double w;
	};
	edge e1, e2;
	e1.up = T0;
	e1.down = T2;
	e1.w = 1 / ((T2.y() - T0.y()) * 1.0 / (T2.x() - T0.x()));
	e2.up = T1;
	e2.down = T2;
	e2.w = 1 / ((T2.y() - T1.y()) * 1.0 / (T2.x() - T1.x()));

	int y = e1.up.y();
	int yMAX = e1.down.y();
	double x1 = e1.up.x();
	double x2 = e2.up.x();

	do {
		if (x1 != x2) {
			for (int i = int(x1 + 0.5); i < int(x2 + 0.5); i++) {
				if (ipolchecked == TRUE)
				{
					if (interpol == 0) {
						setPixel(i, y, nearestNeighbor(QPoint(i, y)));
					}
					else if (interpol == 1) {
						setPixel(i, y, barycentric(QPoint(i, y)));
					}
				}
				else {
					setPixel(i, y, fillColor);
				}
			}
		}
		x1 += e1.w;
		x2 += e2.w;
		y++;
	} while (y < yMAX);

}

void ViewerWidget::ScanLineTriangleUp(QPoint T0, QPoint T1, QPoint T2)
{
	struct edge {
		QPoint up, down;
		double w;
	};
	edge e1, e2;
	e1.up = T0;
	e1.down = T1;
	e1.w = 1 / ((T1.y() - T0.y()) * 1.0 / (T1.x() - T0.x()));
	e2.up = T0;
	e2.down = T2;
	e2.w = 1 / ((T2.y() - T0.y()) * 1.0 / (T2.x() - T0.x()));

	int y = e1.up.y();
	int yMAX = e1.down.y();
	double x1 = e1.up.x();
	double x2 = e2.up.x();

	do {
		if (x1 != x2) {
			for (int i = int(x1 + 0.5); i < int(x2 + 0.5); i++) {
				if (ipolchecked == TRUE)
				{
					if (interpol == 0) {
						setPixel(i, y, nearestNeighbor(QPoint(i, y)));
					}
					else if (interpol == 1) {
						setPixel(i, y, barycentric(QPoint(i, y)));
					}
				}
				else {
					setPixel(i, y, fillColor);
				}
			}
		}
		x1 += e1.w;
		x2 += e2.w;
		y++;
	} while (y < yMAX);
}

QColor ViewerWidget::nearestNeighbor(QPoint p)
{
	double a1, a2, a0;
	a1 = qSqrt(pow((p.x() - ipolT[1].x()), 2) + pow((p.y() - ipolT[1].y()), 2));
	a2 = qSqrt(pow((p.x() - ipolT[2].x()), 2) + pow((p.y() - ipolT[2].y()), 2));
	a0 = qSqrt(pow((p.x() - ipolT[0].x()), 2) + pow((p.y() - ipolT[0].y()), 2));
	if (a0 < a1 && a0 < a2) return color0;
	else if (a1 < a0 && a1 < a2) return color1;
	else if (a2 < a1 && a2 < a0) return color2;
	else return Qt::white;
}

QColor ViewerWidget::barycentric(QPoint p)
{
	QPoint u = ipolT[1] - ipolT[0];
	QPoint v = ipolT[2] - ipolT[0];
	double n = abs(u.x() * v.y() - u.y() * v.x());
	double A = n / 2;
	double A0 = abs((1.0 * (ipolT[1].x() - p.x()) * (ipolT[2].y() - p.y()) - 1.0 * (ipolT[1].y() - p.y()) * (ipolT[2].x() - p.x())) / 2);
	double A1 = abs((1.0 * (ipolT[0].x() - p.x()) * (ipolT[2].y() - p.y()) - 1.0 * (ipolT[0].y() - p.y()) * (ipolT[2].x() - p.x())) / 2);
	double lambda0 = A0 / A;
	double lambda1 = A1 / A;
	double lambda2 = 1 - lambda0 - lambda1;

	return QColor(int(lambda0 * color0.red() + lambda1 * color1.red() + lambda2 * color2.red()), int(lambda0 * color0.green() + lambda1 * color1.green() + lambda2 * color2.green()), int(lambda0 * color0.blue() + lambda1 * color1.blue() + lambda2 * color2.blue()));
}

void ViewerWidget::setFillColor(QColor fColor)
{
	ipolchecked = FALSE;
	fillColor = fColor;
}

void ViewerWidget::setInterpolIndex(int ipol)
{
	interpol = ipol;
	ipolchecked = TRUE;
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}