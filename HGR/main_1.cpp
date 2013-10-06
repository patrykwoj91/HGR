#include "stdafx.h"
using namespace cv;

CvHistogram * S, *N; // histogramy skóry (S) i nieskóry (N)
int nauka; // ile klatek ma trwaæ budowanie histogramów
int przedzial; // ile przedzia³ów ma mieæ histogram
int podzielnik; // szerokoœæ przedzia³u
int piksele; // odleg³oœæ w pikselach od ramki twarzy
int prog; // próg 

/*
 * Funkcja zapisuje w macierzy prostok¹t o przek¹tnej miêdzy p1 i p2 wartoœci¹ val
 * p1 jest lewym górnym punktem, p2 prawym dolnym
 */
void zapiszPunkty(CvMat* img, CvPoint p1, CvPoint p2, float val)
{
	for(int x = p1.x; x < p2.x + 1; x++)
	{
		cvSet2D(img, p1.y, x, cvScalar(val));
		cvSet2D(img, p2.y, x, cvScalar(val));
	}
	for (int y = p1.y; y < p2.y + 1; y++)
	{
		cvSet2D(img, y, p1.x, cvScalar(val));
		cvSet2D(img, y, p2.x, cvScalar(val));
	}
}

/*
 * Tworzy obraz, w którym piksele o wartoœci val ustawione zostan¹
 * na 255 a pozosta³e na 0
 */
IplImage* pobierzMaske(CvMat * ws, int val)
{
	IplImage * maska = cvCreateImage(cvGetSize(ws), 8, 1);

	for (int y = 0; y < ws->height; y++)
	{
		for (int x = 0; x < ws->width; x++)
			if (cvGet2D(ws, y, x).val[0] == val)
				cvSet2D(maska, y, x, cvScalarAll(255.0));
			else
				cvSet2D(maska, y, x, cvScalarAll(0.0));
	}
	return maska;
}

/*
 * Analizuje kolorowy obraz wzglêdem wyników uzyskanych z
 * zakwalifikowania pikseli do odpowiednich klas - uaktualnia histogramy
 * skóry i nieskóry
 */
void analizujDoHistogramu(IplImage* obraz, CvMat* ws)
{
	for (int y = 0; y < obraz->height; y++)
	{
		uchar* ptrObraz = (uchar*) (obraz->imageData + y * obraz->widthStep);
		const int* ptrWS = (const int*) (ws->data.ptr + y * ws->step);
		for (int x = 0; x < obraz->width; x++)
		{
			uchar b, g, r;

			b = ptrObraz[3 * x];
			g = ptrObraz[3 * x + 1];
			r = ptrObraz[3 * x + 2];

			int klasa = *ptrWS++;
			if (klasa == 1)
				(*cvGetHistValue_3D(S, cvRound(b / podzielnik), cvRound(g / podzielnik), cvRound(r / podzielnik)))++;
			else if (klasa == 2)
				(*cvGetHistValue_3D(N, cvRound(b / podzielnik), cvRound(g / podzielnik), cvRound(r / podzielnik)))++;

		}
	}
}

/*
 * Normalizacja histogramów
 * Po tej operacji, wszystkie wartoœci bêd¹ z zakresu 0;1
 * i bêdzie mo¿na je traktowaæ jak wartoœæ prawdopodobieñstwa
 */
void normalizujHistogramy()
{
	for (int b = 0; b < przedzial; b++)
    {
        for (int g = 0; g < przedzial; g++)
        {
            for (int r = 0; r < przedzial; r++)
            {
                double skora = cvQueryHistValue_3D(S, b, g, r);
                double nieSkora = cvQueryHistValue_3D(N, b, g, r);
                double suma = skora + nieSkora;
                if (suma > 0.0)
                {
                    (*cvGetHistValue_3D(S, b, g, r)) = skora / suma;
                    (*cvGetHistValue_3D(N, b, g, r)) = nieSkora / suma;
                }
            }
        }
    }
}

/*
 * Ka¿demu pikselowi przyporz¹dkowuje prawdopodobieñstwo
 * ¿e jest on pikselem skóry i rozci¹ga na zakres 0-255
 * Im jaœniejszy piksel w wyniku, tym bardziej prawdopodobne
 * jest, ¿e jest to piksel skóry
 */
IplImage * mapaPrawdopodobienstwa(IplImage * obraz)
{
    IplImage* mapa = cvCreateImage(cvGetSize(obraz), 8, 1);

    for (int y = 0; y < obraz->height; y++)
    {
        uchar* ptrObraz = (uchar*) (obraz->imageData + y * obraz->widthStep);
        uchar* ptrRet = (uchar*) (mapa->imageData + y * mapa->widthStep);
        for (int x = 0; x < obraz->width; x++)
        {
            uchar b, g, r;

            b = ptrObraz[3 * x];
            g = ptrObraz[3 * x + 1];
            r = ptrObraz[3 * x + 2];

            double p = cvQueryHistValue_3D(S, cvRound(b / podzielnik), cvRound(g / podzielnik), cvRound(r / podzielnik));
            ptrRet[x] = cvRound(p * 255.0);
        }
    }
    return mapa;
}

int main(int argc, char** argv)
{
    printf("Sposób u¿ycia (wszystkie parametry opcjonalne):\n");
    printf("\t[cam] [uczenie] [przedzial] [prog] [piksele]\n");
    printf("\t\tcam -\tnumer kamery\n");
    printf("\t\tuczenie -\tliczba klatek przeznaczonych na uczenie skóry\n");
    printf("\t\tprzedzial -\tliczba przedzia³ów histogramu (potêga 2)\n");
    printf("\t\tprog -\tprogowanie do pikseli o prawdopodobieñstwie prog w zakresie 0-100 lub -1 gdy bez progowania\n");
    printf("\t\tpiksele -\todsuniêcie ramki twarzy w pikselach (zobacz kod by uzyskaæ wiêcej szczegó³ów)\n");

    CvCapture* cam = NULL;

    // czytanie parametrów
    if (argc > 1)
        cam = cvCreateCameraCapture(atoi(argv[1]));
    else
        cam = cvCreateCameraCapture(0);
    if (argc > 2)
        nauka = atoi(argv[2]);
    else
        nauka = 10;
    if (argc > 3)
        przedzial = atoi(argv[3]);
    else
        przedzial = 64;
    if (argc > 4)
        prog = cvRound(atoi(argv[4]) * 2.55);
    else
        prog = -1;
    if (argc > 5)
        piksele = atoi(argv[5]);
    else
        piksele = 10;

    podzielnik = 256 / przedzial;

    // tworzenie histogramów
    int sizes[] = {przedzial, przedzial, przedzial};
    S = cvCreateHist(3, sizes, CV_HIST_ARRAY);
    N = cvCreateHist(3, sizes, CV_HIST_ARRAY);

    cvNamedWindow("detekcja twarzy", CV_WINDOW_AUTOSIZE);
    IplImage* ramka = cvQueryFrame(cam);
    CvMat* ws = cvCreateMat(ramka->height, ramka->width, CV_32SC1);
    IplImage* kopia;
    double fps = 60;
    int odstep_miedzy_klatkami = 1000 / fps;

    CvMemStorage * storage = cvCreateMemStorage(0);
    CvHaarClassifierCascade * haar = (CvHaarClassifierCascade*) cvLoad("/usr/local/share/opencv/haarcascades/haarcascade_frontalface_alt.xml");

    // pusta pêtla - dajemy kamerze czas na przystosowanie
    // siê do warunków oœwietleniowych
    while (true)
    {
        ramka = cvQueryFrame(cam);
        if (ramka == 0)
            break;

        cvShowImage("detekcja twarzy", ramka);
        int c = cvWaitKey(odstep_miedzy_klatkami);
        if (c == 'n')
            break;
    }

    // proces wype³nania histogramów
    while (nauka > 0)
    {
        ramka = cvQueryFrame(cam);
        if (ramka == 0)
            break;
        cvClearMemStorage(storage);
        kopia = cvCloneImage(ramka);

        // szukanie twarzy - by³o ju¿ kiedyœ na blogu
        double skala = 2.0;
        IplImage *temp = cvCreateImage(cvSize(ramka->width, ramka->height), 8, 1);
        IplImage *temp2 = cvCreateImage(cvSize(cvRound(ramka->width / skala), cvRound(ramka->height / skala)), 8, 1);
        cvConvertImage(ramka, temp, CV_BGR2GRAY);
        cvResize(temp, temp2, CV_INTER_LINEAR);
        CvSeq *wynik = cvHaarDetectObjects(temp2, haar, storage, 1.1, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize(50, 50));
        cvZero(ws);

        // analizujemy tylko pierwsz¹ twarz
        if (wynik->total > 0)
        {
            CvRect * twarz = (CvRect*) cvGetSeqElem(wynik, 0);
			
			// ustalamy dwa prostok¹ty - wewnêtrzny i zewnêtrzny
			// s¹ to prostok¹ty twarzy których wszystkie boki s¹ odsuniête
			// o wartoœæ parametru piksele do wewn¹trz/na zewn¹trz
			// dla obszaru twarzy/nietwarzy
			CvPoint mP1, mP2, dP1, dP2;
			dP1.x = mP1.x = twarz->x * skala;
			dP1.y = mP1.y = twarz->y * skala;
			dP2.x = mP2.x = (twarz->x + twarz->width) * skala;
			dP2.y = mP2.y = (twarz->y + twarz->height) * skala;
			mP1.x += skala * piksele;
			mP1.y += skala * piksele;
			mP2.x -= skala * piksele;
			mP2.y -= skala * piksele;
			dP1.x -= skala * piksele;
			dP1.y -= skala * piksele;
			dP2.x += skala * piksele;
			dP2.y += skala * piksele;

			// zapisujemy prostok¹ty do macierzy poddawanej operacji watershed
			zapiszPunkty(ws, mP1, mP2, 1);
			zapiszPunkty(ws, dP1, dP2, 2);
            cvWatershed(ramka, ws);
            // analizujemy wyniki
            analizujDoHistogramu(ramka, ws);

            // poni¿ej kolorowanie obrazu - tylko do celów obejrzenia wyników :)
            IplImage* maska1 = pobierzMaske(ws, 1);
            IplImage* maska2 = pobierzMaske(ws, 2);

            cvAddS(ramka, cvScalar(0.0, 205.0, 0.0, 0.0), kopia, maska1);
            cvAddS(ramka, cvScalar(0.0, 0.0, 205.0, 0.0), kopia, maska2);

            cvReleaseImage(&maska1);
            cvReleaseImage(&maska2);

        }

        cvShowImage("detekcja twarzy", kopia);
        cvReleaseImage(&temp);
        cvReleaseImage(&kopia);
        cvReleaseImage(&temp2);
        int c = cvWaitKey(odstep_miedzy_klatkami);
        if (c == 'k')
            break;
        nauka--;
    }

    cvReleaseHaarClassifierCascade(&haar);
    cvNamedWindow("skora", CV_WINDOW_AUTOSIZE);

    // normalizujemy przygotowane histogramy
    normalizujHistogramy();

    // pêtla g³ówna
    while (true)
    {
        ramka = cvQueryFrame(cam);
        if (ramka == 0)
            break;

        // ustalamy prawdopodobieñstwa
        IplImage* mapa = mapaPrawdopodobienstwa(ramka);
        // je¿eli podaliœmy, ¿e chcemy progowaæ to progujemy
        if (prog > 0)
            cvThreshold(mapa, mapa, prog, 255, CV_THRESH_BINARY);
        cvShowImage("skora", mapa);
        cvReleaseImage(&mapa);
        int c = cvWaitKey(odstep_miedzy_klatkami);
        if (c == 'k')
            break;
    }

    cvReleaseMat(&ws);
    cvReleaseHist(&N);
    cvReleaseHist(&S);
    cvDestroyAllWindows();
    cvReleaseCapture(&cam);

    return EXIT_SUCCESS;
}
