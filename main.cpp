#include <curl/curl.h>
#include <stdlib.h>	  
#include <stdio.h>	  
#include <string.h>	  
#include <graphics.h> 
#include <conio.h>
#include <math.h>
#include <fcntl.h>	
#include <unistd.h>


char *return_value;	   // global
int return_value_size; // global

int **wnd_array;	  // global
int **wnd_array_copy; // copy wnd_array - global
int **wnd_array_copy_real;
int **wnd_array_copy2;	  // second copy wnd_array - global
int **wnd_array_optimize; // wnd_array for optimize - global

#define window_height 900
#define window_width 1100

void print_array(char **arr)
{
	for (int i = 0; arr[i] != NULL; i++)
	{
		printf("%s\n", arr[i]);
	}
}

void print_array2(int *arr)
{
	int i = 0;
	for (; arr[i] != -1; i++)
	{
		printf("%d\n", arr[i]);
	}
	printf("%d\n", arr[i]);
}

struct nokta
{
	double x;
	double y;
	struct nokta *next;
};

struct cizgi
{
	double x1;
	double x1_pixel;
	double y1;
	double y1_pixel;

	double x2;
	double x2_pixel;
	double y2;
	double y2_pixel;

	int sekil_no;

	double egim;
	int egim_pozitif;

	struct cizgi *next;
};

struct satir
{
	char *satir_data;
	char *c_satir_data;
	int *sekil_eleman_tutucu;
	int satir_no;
	int nokta_sayisi;
	int sekil_sayisi;

	struct nokta *nokta_listesi;
	struct cizgi *cizgi_listesi;
	struct satir *next;
};

char *strjoin(const char *s1, const char *s2)
{
	char *str;
	size_t i;
	size_t j;
	size_t x;

	if (!s1 || !s2)
		return (NULL);
	x = (strlen(s1) + strlen(s2));
	str = (char *)malloc(sizeof(char) * x + 1);
	if(str == NULL)
	{
		printf("malloc error\n");
		exit(1);
	}
	i = -1;
	j = -1;
	if (!str)
		return (NULL);
	while (s1[++i])
		str[i] = s1[i];
	while (s2[++j])
		str[i++] = s2[j];
	str[i] = '\0';
	return (str);
}

void fill_square() // beyaz cizgileri yazdirma
{
	int a = 0;
	int b = 0;
	while (a < window_height)
	{
		b = 0;
		while (b < window_width)
		{
			if (a % 10 == 0)
				putpixel(b, a, WHITE);
			else if (b % 10 == 0)
			{
				putpixel(b, a, WHITE);
			}
			b++;
		}
		a++;
	}
}

size_t WriteMemoryCallback(char *buf, size_t size, size_t num_item, void *s)
{
	(void)s;
	// printf("size : %zu\n",size); 1
	// printf("num_item : %zu\n",num_item); 99

	size_t bytes = size * num_item;
	return_value = (char *)malloc(bytes);
	if (return_value == NULL) 
	{
		printf("malloc error\n");
		exit(1);
	}
	
	int newline_flag = 0;
	for (size_t i = 0; i < bytes; i++) // using size_t cuz -w... flag
	{
		if (buf[i] == '\n')
		{
			return_value_size++;
			newline_flag = 1;
		}
		return_value[i] = buf[i]; // bufdan alinan veriler teker teker global degiskene yazilir
	}
	if (newline_flag == 1) // eger tek satir alirsa newline 0 olacagindan buraya girmez ama oldugunda \n sayisi +1 bolunme olacagindan +1 burada eklenir
		return_value_size++;
	return bytes;
}

int count(char *str, char m)
{
	int a = 0, ret = 0;
	while (str[a] != '\0')
	{
		if (str[a] == m)
		{
			ret += 1;
		}
		a++;
	}
	return (ret);
}

int count_int(int *arr, char m)
{
	int size = sizeof(arr) / sizeof(arr[0]);
	int a = 0, ret = 0;
	while (a < size)
	{
		if (arr[a] == m)
		{
			ret += 1;
		}
		a++;
	}
	return (ret);
}

int wnd_count(int m)
{
	int satir_sayisi = window_height / 10;
	int sutun_sayisi = window_width / 10;
	int a = 0;
	int sayici = 0;
	while (a < satir_sayisi)
	{
		int b = 0;
		while (b < sutun_sayisi)
		{
			if (wnd_array[a][b] == m)
			{
				sayici++;
			}
			b++;
		}
		a++;
	}
	return sayici;
}

int wnd_copy_count(int m)
{
	int satir_sayisi = window_height / 10;
	int sutun_sayisi = window_width / 10;
	int a = 0;
	int sayici = 0;
	while (a < satir_sayisi)
	{
		int b = 0;
		while (b < sutun_sayisi)
		{
			if (wnd_array_copy[a][b] == m)
			{
				sayici++;
			}
			b++;
		}
		a++;
	}
	return sayici;
}

void print_wnd()
{
	int satir_sayisi = window_height / 10;
	int sutun_sayisi = window_width / 10;
	int a = 0;
	int sayici = 0;
	while (a < satir_sayisi)
	{
		int b = 0;
		while (b < sutun_sayisi)
		{
			printf("%d ", wnd_array[a][b]);
			b++;
		}
		printf("\n");
		a++;
	}
}

void print_dot(double x, double y)
{
	char charTemp1[50]; // double to array
	snprintf(charTemp1, sizeof(charTemp1), "%.1f", x);
	char charTemp2[50];
	snprintf(charTemp2, sizeof(charTemp2), "%.1f", y);
	char *combinedString = (char *)malloc(strlen(charTemp1) + strlen(charTemp2) + 4); // bu ikisinden string üretme +4 ->( ve ) ve , ve null
	if (combinedString == NULL) 
	{
		printf("malloc error\n");
		exit(1);
	}
	int bas = 0;
	combinedString[bas] = '(';
	bas++;
	for (size_t i = 0; i < strlen(charTemp1); i++)
	{
		combinedString[bas] = charTemp1[i];
		bas++;
	}
	combinedString[bas] = ',';
	bas++;
	for (size_t i1 = 0; i1 < strlen(charTemp2); i1++)
	{
		combinedString[bas] = charTemp2[i1];
		bas++;
	}
	combinedString[bas] = ')';
	bas++;
	combinedString[bas] = '\0';
	if (x * 10 - 10 >= 0 && y * 10 - 20 >= 0) // text yerine 0'a gore kontrol
	{
		settextstyle(2, HORIZ_DIR, 5); // HORIZ_DIRECTION : Yatay
		// printf("deneme\n");
		setcolor(BLUE);
		outtextxy(x * 10 - 10, y * 10 - 20, combinedString);
	}

	// noktayi bastirma tam pixele göre x ve y'ye gore -4 + 4 olacak sekilde cizilir
	setcolor(RED);
	int a = -4;
	while (a <= 4)
	{
		int b = -4;
		while (b <= 4)
		{
			if (x + a >= 0 && y + b >= 0)
			{
				putpixel((x * 10) + a, (y * 10) + b, RED);
			}
			b++;
		}
		a++;
	}
}

void print_dots(struct satir *s)
{
	while (s != NULL)
	{
		struct nokta *nokta_ic = s->nokta_listesi;
		while (nokta_ic != NULL)
		{
			print_dot(nokta_ic->x, nokta_ic->y);
			nokta_ic = nokta_ic->next;
		}
		s = s->next;
	}
}

void print_lines2(struct satir *s)
{
	int kacinci_sekil = 1;
	setcolor(GREEN);
	while (s != NULL)
	{
		struct cizgi *tutucu = s->cizgi_listesi;
		while(tutucu != NULL)
		{
			line(tutucu->x1_pixel,tutucu->y1_pixel,tutucu->x2_pixel,tutucu->y2_pixel);
			tutucu= tutucu->next;
		}
		s = s->next;
	}
}

void print_lines(struct satir *s)
{
	int kacinci_sekil = 1;
	while (s != NULL)
	{
		size_t i = 0;
		struct nokta *tutucu = s->nokta_listesi;
		s->cizgi_listesi = NULL;
		while (s->sekil_eleman_tutucu[i] != -1905)
		{
			// diziye index degerlerini yerlestirmistik tamamlayana kadar alttaki dondu devam eder
			int son = s->sekil_eleman_tutucu[i];
			int bas = s->sekil_eleman_tutucu[i + 1];
			while (son > bas)
			{
				setcolor(GREEN);
				line(tutucu->x * 10, tutucu->y * 10, tutucu->next->x * 10, tutucu->next->y * 10);
				struct cizgi *newCizgi = (struct cizgi *)malloc(sizeof(struct cizgi));
				if (newCizgi == NULL) 
				{
					printf("malloc error\n");
					exit(1);
				}
				newCizgi->x1 = tutucu->x;
				newCizgi->x1_pixel = tutucu->x * 10;
				newCizgi->y1 = tutucu->y;
				newCizgi->y1_pixel = tutucu->y * 10;

				newCizgi->x2 = tutucu->next->x;
				newCizgi->x2_pixel = tutucu->next->x * 10;
				newCizgi->y2 = tutucu->next->y;
				newCizgi->y2_pixel = tutucu->next->y * 10;
				newCizgi->sekil_no = kacinci_sekil;

				// egim bulma
				if (newCizgi->x2 - newCizgi->x1 != 0)
					newCizgi->egim = ((newCizgi->y2 - newCizgi->y1) / (newCizgi->x2 - newCizgi->x1));
				else // 1/0 durumu icin
					newCizgi->egim = -1905;
				newCizgi->next = s->cizgi_listesi;
				s->cizgi_listesi = newCizgi;
				son--;
				tutucu = tutucu->next; // nokta listesini gezer
			}
			if (tutucu->next) // birden fazla sekiller icin
				tutucu = tutucu->next;
			// printf("**%d %d\n\n",son,bas);
			kacinci_sekil += 1;
			i += 2;
		}
		s = s->next;
	}
}

void kare_doldurma(double x, double y, int color)
{
	int x_ = int(x);
	int y_ = int(y);

	setcolor(WHITE); // Kare rengi
	rectangle(x_, y_, x_ + 10, y_ + 10);
	setfillstyle(SOLID_FILL, color); // Kareyi doldurmak i�in rengi ayarla
	floodfill((x_ + x_ + 10) / 2, (y_ + y_ + 10) / 2, WHITE);
}

void kare_duzeltme(double x, double y)
{
	int x_ = x;
	int y_ = y;

	setcolor(WHITE); // Kare rengi
	rectangle(x_, y_, x_ + 10, y_ + 10);
	setfillstyle(SOLID_FILL, BLACK); // Kareyi doldurmak i�in rengi ayarla
	floodfill((x_ + x_ + 10) / 2, (y_ + y_ + 10) / 2, WHITE);
}

void cizgi_karelerini_doldurma(struct satir *s, int color)
{
	while (s != NULL)
	{
		struct cizgi *tutucu = s->cizgi_listesi;
		double onceki_x2 = 1000; // taban ve sa� kenar tasmasini engellemek icin
		double onceki_y2 = 1000;
		while (tutucu != NULL)
		{
			double x1_boya = tutucu->x1, y1_boya = tutucu->y1;
			double egim = pow(tutucu->egim, -1);
			double egim_temp = egim;
			if (tutucu->egim > 0 && tutucu->x1 > tutucu->x2)
			{
				// printf("egim > 0 ve x1 > x2 \n");
				// printf("*%f %f %f %f\n",x1_boya,y1_boya,egim,egim_temp);
				int ilk_adim = 1;
				while (x1_boya > tutucu->x2 && y1_boya > tutucu->y2)
				{
					// printf("?: %f %f\n",x1_boya,y1_boya);
					int boya_x = x1_boya;
					int boya_y = y1_boya;
					if (egim == 1) // istedigim gibi olmuyordu
					{
						kare_doldurma((boya_x - 1) * 10, (boya_y - 1) * 10, color);
						// printf("iceriyi BOYADIM: %d %d\n",(boya_x-1)*10,boya_y*10);
						wnd_array[boya_y - 1][boya_x - 1] = tutucu->sekil_no;
						x1_boya -= 1;
						boya_x -= 1;
						y1_boya -= 1;
					}
					else
					{
						int boya_gezici = 0;
						if (ilk_adim)
						{
							boya_x -= 1;
							ilk_adim = 0;
						}
						if (egim < 0)
							egim = egim * -1;
						while (boya_gezici < egim)
						{

							kare_doldurma(boya_x * 10, (boya_y - 1) * 10, color); // -" olmazsa bir alt�ndaki kareyi boyuyor
							wnd_array[boya_y - 1][boya_x] = tutucu->sekil_no;
							boya_x -= 1;
							boya_gezici += 1;
						}

						x1_boya -= egim;
						if (x1_boya < boya_x)
						{
							int tmp_x1_b = x1_boya;
							kare_doldurma(tmp_x1_b * 10, boya_y * 10, color);
							wnd_array[boya_y][tmp_x1_b] = tutucu->sekil_no;
						}
						y1_boya -= 1;
					}
				}
			}
			else if (tutucu->egim > 0 && tutucu->x1 < tutucu->x2)
			{
				while (x1_boya < tutucu->x2 && y1_boya < tutucu->y2)
				{
					int boya_x = x1_boya;
					int boya_y = y1_boya;
					int boya_gezici = 0;
					while (boya_gezici < egim)
					{

						kare_doldurma(boya_x * 10, boya_y * 10, color);
						wnd_array[boya_y][boya_x] = tutucu->sekil_no;
						boya_x += 1;

						boya_gezici += 1;
					}

					x1_boya += egim;
					if (x1_boya > boya_x)
					{
						int tmp_x1_b = x1_boya;
						kare_doldurma(tmp_x1_b * 10, boya_y * 10, color);
						wnd_array[boya_y][tmp_x1_b] = tutucu->sekil_no;
					}
					y1_boya += 1;
				}
			}
			else if (tutucu->egim < 0 && tutucu->x1 > tutucu->x2)
			{

				// printf("egim < 0 ve x1 > x2 \n");
				int ilk_adim = 1;
				while (x1_boya > tutucu->x2 && y1_boya < tutucu->y2)
				{
					// printf("?: %f %f\n",x1_boya,y1_boya);
					int boya_x = x1_boya;
					int boya_y = y1_boya;
					if (egim == -1) // istedigim gibi olmuyordu
					{
						kare_doldurma((boya_x - 1) * 10, boya_y * 10, color);
						wnd_array[boya_y][boya_x - 1] = tutucu->sekil_no;
						x1_boya -= 1;
						boya_x -= 1;
						y1_boya += 1;
					}
					else
					{
						int boya_gezici = 0;
						if (ilk_adim)
						{
							boya_x -= 1;
							ilk_adim = 0;
						}
						if (egim < 0)
							egim = egim * -1;
						while (boya_gezici < egim)
						{

							kare_doldurma(boya_x * 10, boya_y * 10, color); // -" olmazsa bir altindaki kareyi boyuyor
							wnd_array[boya_y][boya_x] = tutucu->sekil_no;
							boya_x -= 1;

							boya_gezici += 1;
						}

						x1_boya -= egim;
						if (x1_boya > boya_x)
						{
							int tmp_x1_b = x1_boya;
							kare_doldurma(tmp_x1_b * 10, boya_y * 10, color);
							wnd_array[boya_y][tmp_x1_b] = tutucu->sekil_no;
						}
						y1_boya += 1;
					}
				}
			}
			else if (tutucu->egim < 0 && tutucu->x1 < tutucu->x2)
			{
				// printf("egim < 0 ve x1 < x2 \n");
				while (x1_boya < tutucu->x2 && y1_boya > tutucu->y2)
				{
					// printf("?: %f %f\n",x1_boya,y1_boya);
					int boya_x = x1_boya;
					int boya_y = y1_boya;
					int boya_gezici = 0;
					if (egim < 0)
						egim = egim * -1;
					while (boya_gezici < egim)
					{

						kare_doldurma(boya_x * 10, (boya_y - 1) * 10, color); // -" olmazsa bir altindaki kareyi boyuyor
						wnd_array[boya_y - 1][boya_x] = tutucu->sekil_no;
						boya_x += 1;

						boya_gezici += 1;
					}

					x1_boya += egim;
					if (x1_boya > boya_x)
					{
						int tmp_x1_b = x1_boya;
						kare_doldurma(tmp_x1_b * 10, (boya_y - 1) * 10, color);
						wnd_array[boya_y - 1][tmp_x1_b] = tutucu->sekil_no;
					}
					y1_boya -= 1;
				}
			}
			else if (tutucu->egim == 0) 
			{
				int boya_x = x1_boya;
				int boya_y = y1_boya;
				if (tutucu->x1 < tutucu->x2)
				{
					if (onceki_y2 != 1000 && onceki_y2 < tutucu->y1)
						boya_y--;
					while (x1_boya < tutucu->x2)
					{
						kare_doldurma(boya_x * 10, boya_y * 10, color);
						wnd_array[boya_y][boya_x] = tutucu->sekil_no;
						x1_boya += 1;
						boya_x += 1;
					}
				}
				else if (tutucu->x1 > tutucu->x2)
				{
					if (onceki_y2 != 1000 && onceki_y2 < tutucu->y1)
						boya_y--;
					boya_x--; // hata fix yanlis cizdiriyordu sol kisimda oldugu icin
					while (x1_boya > tutucu->x2)
					{
						kare_doldurma(boya_x * 10, boya_y * 10, color);
						wnd_array[boya_y][boya_x] = tutucu->sekil_no;
						x1_boya -= 1;
						boya_x -= 1;
					}
				}
			}
			else if (tutucu->egim == -1905)
			{
				int boya_x = x1_boya;
				int boya_y = y1_boya;
				if (tutucu->y1 < tutucu->y2)
				{
					if (onceki_x2 != 1000 && onceki_x2 < tutucu->x1) // dikdortgen sagi icin
						boya_x--;
					while (boya_y < tutucu->y2)
					{
						kare_doldurma(boya_x * 10, boya_y * 10, color);
						wnd_array[boya_y][boya_x] = tutucu->sekil_no;
						y1_boya++;
						boya_y++;
					}
				}
				else if (tutucu->y1 > tutucu->y2)
				{
					if (onceki_x2 != 1000 && onceki_x2 < tutucu->x1) // dikdortgen sagi icin
						boya_x--;
					while (boya_y > tutucu->y2)
					{
						kare_doldurma(boya_x * 10, (boya_y - 1) * 10, color);
						wnd_array[boya_y - 1][boya_x] = tutucu->sekil_no;
						y1_boya--;
						boya_y--;
					}
				}
			}
			onceki_x2 = tutucu->x2;
			onceki_y2 = tutucu->y2;
			tutucu = tutucu->next;
		}
		s = s->next;
	}
}

int sekil_sayisi_hesaplama(struct satir *s)
{
	int c = 0;
	while (s != NULL)
	{
		c += s->sekil_sayisi;
		s = s->next;
	}
	return c;
}

void kare_iclerini_doldurma(int sekil_sayisi, int color)
{
	int bas = 1;
	int satir_sayisi = window_height / 10;
	int sutun_sayisi = window_width / 10;
	while (bas <= sekil_sayisi) // tum sekilleri gezecek
	{
		int a = 0;
		int flag_ulasma = 0;
		while (a < satir_sayisi)
		{
			int b = 0;
			if (a + 1 < satir_sayisi)
			{
				int sayan = 0;
				while (b < sutun_sayisi)
				{
					if (wnd_array[a + 1][b] == bas)
					{
						sayan++;
					}
					b++;
				}
				if (sayan > 0)
				{
					flag_ulasma = 1;
				}
				if (sayan == 0 && flag_ulasma == 1) // en son cizilecek yden bir sonraki satira ulastik mi kontrolu eger ulastiysak direkt cik
				{
					break;
				}
			}
			b = 0;
			while (b < sutun_sayisi)
			{
				if (a - 1 >= 0 && wnd_array[a - 1][b] == bas && b - 1 >= 0 && wnd_array[a][b - 1] == bas && wnd_array[a][b] != bas) // solum ve ustum doluysa ve su an oldugum eger o sekil_no'ya esit degilse boya
				{
					kare_doldurma(b * 10, a * 10, color);
					wnd_array[a][b] = bas;
					if (wnd_array[a][b + 1] == bas) // ta ki en sagdaki boyanmis sekil noyu gorene kadar ilerle
					{
						break;
					}
				}
				b++;
			}
			a++;
		}
		bas++;
	}
}

int birlik_ciz(int bas_x, int bas_y, int son_x, int son_y, int sekil_no)
{
	// printf("1\n");
	if (bas_y + 1 > window_height / 10 || bas_x + 1 > window_width / 10)
	{
		// printf("%d numarali sekil icin istenilen 1luk kare cizilemiyor\n",sekil_no);
		return (-1);
	}

	int bas_y_1 = bas_y;
	while (bas_y_1 < bas_y + 1)
	{
		int bas_x_1 = bas_x;
		while (bas_x_1 < bas_x + 1)
		{
			
			if (wnd_array_copy[bas_y_1][bas_x_1] != -1 && wnd_array_copy[bas_y_1][bas_x_1] != sekil_no )
			{
				// printf("%d numarali sekil icin istenilen 1luk kare cizilemiyor\n",sekil_no);
				return (-1);
			}
			bas_x_1++;
		}
		bas_y_1++;
	}

	// printf("%d. sekil icin 1'luk kare cizilmeye uygun!\n",sekil_no);
	int y_bas_temp = bas_y;
	while (y_bas_temp < bas_y + 1)
	{
		int x_bas_temp = bas_x;
		while (x_bas_temp < bas_x + 1)
		{
			kare_doldurma(x_bas_temp * 10, y_bas_temp * 10, 14);
			wnd_array_copy[y_bas_temp][x_bas_temp] = 104 + 100 * (sekil_no - 1);
			x_bas_temp++;
		}
		y_bas_temp++;
	}
	return (1);
}

int ikilik_ciz(int bas_x, int bas_y, int son_x, int son_y, int sekil_no)
{
	// printf("16\n");

	// kontrol kismi
	if (bas_y + 2 > window_height / 10 || bas_x + 2 > window_width / 10)
	{
		// printf("%d numarali sekil icin istenilen 2luk kare cizilemiyor\n",sekil_no);
		return (-1);
	}

	int bas_y_1 = bas_y;
	while (bas_y_1 < bas_y + 2)
	{
		int bas_x_1 = bas_x;
		while (bas_x_1 < bas_x + 2)
		{
			if (wnd_array_copy[bas_y_1][bas_x_1] != -1 && wnd_array_copy[bas_y_1][bas_x_1] != sekil_no /* && !(wnd_array_copy[bas_y_1][bas_x_1] >= (100+ 100*(sekil_no-1)) && wnd_array_copy[bas_y_1][bas_x_1] <= (104 + 100*(sekil_no-1))*/)
			{
				return (-1);
			}
			bas_x_1++;
		}
		bas_y_1++;
	}
	// printf("%d. sekil icin 16'luk kare cizilmeye uygun!\n",sekil_no);
	int y_bas_temp = bas_y;
	while (y_bas_temp < bas_y + 2)
	{
		int x_bas_temp = bas_x;
		while (x_bas_temp < bas_x + 2)
		{
			kare_doldurma(x_bas_temp * 10, y_bas_temp * 10, 9);
			wnd_array_copy[y_bas_temp][x_bas_temp] = 103 + 100 * (sekil_no - 1);
			x_bas_temp++;
		}
		y_bas_temp++;
	}
	return (1);
}

int dortluk_ciz(int bas_x, int bas_y, int son_x, int son_y, int sekil_no)
{
	// printf("4\n");

	// kontrol kismi
	if (bas_y + 4 > window_height / 10 || bas_x + 4 > window_width / 10)
	{
		// printf("%d numarali sekil icin istenilen 4luk kare cizilemiyor\n",sekil_no);
		return (-1);
	}

	int bas_y_1 = bas_y;
	while (bas_y_1 < bas_y + 4)
	{
		int bas_x_1 = bas_x;
		while (bas_x_1 < bas_x + 4)
		{
			if (wnd_array_copy[bas_y_1][bas_x_1] != -1 && wnd_array_copy[bas_y_1][bas_x_1] != sekil_no /* && !(wnd_array_copy[bas_y_1][bas_x_1] >= (100+ 100*(sekil_no-1)) && wnd_array_copy[bas_y_1][bas_x_1] <= (104 + 100*(sekil_no-1)))*/)
			{
				return (-1);
			}
			bas_x_1++;
		}
		bas_y_1++;
	}

	// printf("%d. sekil icin 4'luk kare cizilmeye uygun!\n",sekil_no);
	int y_bas_temp = bas_y;
	while (y_bas_temp < bas_y + 4)
	{
		int x_bas_temp = bas_x;
		while (x_bas_temp < bas_x + 4)
		{
			kare_doldurma(x_bas_temp * 10, y_bas_temp * 10, 12);
			wnd_array_copy[y_bas_temp][x_bas_temp] = 102 + 100 * (sekil_no - 1);
			x_bas_temp++;
		}
		y_bas_temp++;
	}
	return (1);
}

int sekizlik_ciz(int bas_x, int bas_y, int son_x, int son_y, int sekil_no)
{
	// printf("16\n");

	// kontrol kismi
	if (bas_y + 8 > window_height / 10 || bas_x + 8 > window_width / 10)
	{
		// printf("%d numarali sekil icin istenilen 8luk kare cizilemiyor\n",sekil_no);
		return (-1);
	}

	int bas_y_1 = bas_y;
	while (bas_y_1 < bas_y + 8)
	{
		int bas_x_1 = bas_x;
		while (bas_x_1 < bas_x + 8)
		{
			if (wnd_array_copy[bas_y_1][bas_x_1] != -1 && wnd_array_copy[bas_y_1][bas_x_1] != sekil_no /*&& !(wnd_array_copy[bas_y_1][bas_x_1] >= (100+ 100*(sekil_no-1)) && wnd_array_copy[bas_y_1][bas_x_1] <= (104 + 100*(sekil_no-1)))*/)
			{
				return (-1);
			}
			bas_x_1++;
		}
		bas_y_1++;
	}
	// printf("%d. sekil icin 8'luk kare cizilmeye uygun!\n",sekil_no);
	int y_bas_temp = bas_y;
	while (y_bas_temp < bas_y + 8)
	{
		int x_bas_temp = bas_x;
		while (x_bas_temp < bas_x + 8)
		{
			kare_doldurma(x_bas_temp * 10, y_bas_temp * 10, 11);
			wnd_array_copy[y_bas_temp][x_bas_temp] = 101 + 100 * (sekil_no - 1);
			x_bas_temp++;
		}
		y_bas_temp++;
	}
	return (1);
}

int on_altilik_ciz(int bas_x, int bas_y, int son_x, int son_y, int sekil_no)
{
	// printf("16\n");

	// kontrol kismi
	
	
	if (bas_y + 16 > window_height / 10 || bas_x + 16 > window_width / 10)
	{
		// printf("%d numarali sekil icin istenilen 16luk kare cizilemiyor\n",sekil_no);
		return (-1);
	}

	int bas_y_1 = bas_y;
	while (bas_y_1 < bas_y + 16)
	{
		int bas_x_1 = bas_x;
		while (bas_x_1 < bas_x + 16)
		{
			if (wnd_array_copy[bas_y_1][bas_x_1] != -1 && wnd_array_copy[bas_y_1][bas_x_1] != sekil_no /*&& !(wnd_array_copy[bas_y_1][bas_x_1] >= (100+ 100*(sekil_no-1)) && wnd_array_copy[bas_y_1][bas_x_1] <= (104 + 100*(sekil_no-1)))*/)
			{
				// printf("%d numarali sekil icin istenilen 16luk kare cizilemiyor\n",sekil_no);
				if(bas_y_1 >= (son_y-bas_y)/2 && bas_x_1 >= (son_x-bas_x)/2 && bas_x-1 >= 0 && bas_y-1 >= 0)
				{
					int a = on_altilik_ciz(bas_x-1,bas_y-1,son_x-1,son_y-1,sekil_no);
					if(a == 1)
						return(1);
				}
				else if(bas_y_1 >= (son_y-bas_y)/2 && bas_x_1 < (son_x-bas_x)/2 && son_x+1 < window_width && bas_y-1 >= 0)
				{
					int b = on_altilik_ciz(bas_x+1,bas_y-1,son_x+1,son_y-1,sekil_no);
					if(b == 1)
						return(1);
				}
				else if(bas_y_1 < (son_y-bas_y)/2 && bas_x_1 >= (son_x-bas_x)/2 && bas_x-1 >= 0 && son_y+1 < window_height)
				{
					int c = on_altilik_ciz(bas_x-1,bas_y+1,son_x-1,son_y+1,sekil_no);
					if(c == 1)
						return(1);
				}
				else if(bas_y_1 < (son_y-bas_y)/2 && bas_x_1 < (son_x-bas_x)/2 && son_x+1 < window_width && son_y+1 < window_height)
				{
					int d = on_altilik_ciz(bas_x+1,bas_y+1,son_x+1,son_y+1,sekil_no);
					if(d == 1)
						return(1);
				}
				else
				{
					//printf("%d numarali sekil icin istenilen 16l�k kare cizilemiyor\n",sekil_no);
					return (-1);
				}
				return (-1);
			}
			bas_x_1++;
		}
		bas_y_1++;
	}
	// printf("%d. sekil icin 16'luk kare cizilmeye uygun!\n",sekil_no);
	int y_bas_temp = bas_y;
	while (y_bas_temp < bas_y + 16)
	{
		int x_bas_temp = bas_x;
		while (x_bas_temp < bas_x + 16)
		{
			kare_doldurma(x_bas_temp * 10, y_bas_temp * 10, 10);
			wnd_array_copy[y_bas_temp][x_bas_temp] = 100 + 100 * (sekil_no - 1);
			x_bas_temp++;
		}
		y_bas_temp++;
	}
	return (1);
}

void pencere_temizleme()
{
	printf("Pencere temizleniyor!\n");
	int y = 0;
	while (y < window_height)
	{
		int x = 0;
		while (x < window_width)
		{
			putpixel(x, y, BLACK);
			x++;
		}
		y++;
	}
}

int main()
{
	initwindow(window_width, window_height);

	printf("Welcome to our PROLAB1's first project\n");
	printf("Made by Sefa TUNCA and Ardahan AYTAN\n\n");

	CURL *curl = curl_easy_init(); // curl oturumu ba�latma - sonraki freeyi kullanabilmek i�in bunu yukar� aldim yoksa if blokta tan�mland���ndan hata veriyordu

	printf("Mod secimi;\n");
	printf("1 - Siteden data alma modu\n");
	printf("2- Kullanicidan data alma modu\n");
	printf("3- Dosyadan okuma modu\n");
	int mode;
	printf("Mod :");
	scanf("%d", &mode);

	if (mode == 1)
	{
		// curl devami
		printf("CURL modu secildi\n");

		if (curl == NULL) // curl oturumu ba�lat�lamad�
		{
			printf("curl failed\n");
			return 0;
		}
		// set
		curl_easy_setopt(curl, CURLOPT_URL, "http://bilgisayar.kocaeli.edu.tr/prolab1/prolab1.txt");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		// perform
		CURLcode result = curl_easy_perform(curl); // transfer i�lemi - bu olmadan calismaz asil islem bu usttekiler bunun setupiydi.
		if (result != CURLE_OK)
		{
			printf("Indirme hatasi : %s\n", curl_easy_strerror(result));
			return 0;
		}
		else
		{
			printf("Indirme basarili\n");
		}
	}
	else if (mode == 2)
	{
		printf("Kullanicidan data alma modu secildi\n");

		printf("Kac satir girilecek : ");
		scanf("%d", &return_value_size);
		if (return_value_size <= 0)
		{
			printf("Yanlis satir sayisi girisi\n");
			return 0;
		}
		char user_data[900];
		int user_data_index = 0;
		return_value = (char *)malloc(sizeof(char) * 2);
		if(return_value == NULL)
		{
			printf("malloc error\n");
			exit(1);
		}
		return_value[0] = '-';
		return_value[1] = '\0';
		while (user_data_index < return_value_size)
		{
			printf("%d. satir : ", user_data_index + 1);
			scanf("%s", user_data);
			return_value = strjoin(return_value, user_data);
			if (user_data_index != return_value_size - 1) // son alinacak satir degil ise araya \n ekledik
				return_value = strjoin(return_value, "\n");
			user_data_index++;
		}

		char *t4 = strdup(return_value + 1); // strjoin hatas�ndan ka�mak i�in kulland���mz -'yi at�yoruz att�k
		return_value = t4;
	}
	else if (mode == 3)
	{
		int dosya_fd;
		char buffer;					// tampon
		char *dosya_icerik = NULL;		// Dosyan�n i�eri�ini saklayacak karakter dizisi
		ssize_t okunan_byte_sayisi = 0; // Okunan byte say�s�
		ssize_t dosya_boyutu = 0;		// Dosyan�n boyutu
		int newline_sayisi = 0;

		dosya_fd = open("dosya.txt", O_RDONLY);
		if (dosya_fd == -1)
		{
			perror("Dosya acma hatasi\n");
			return (1);
		}

		// Dosyanin boyutunu alma islemi
		dosya_boyutu = lseek(dosya_fd, 0, SEEK_END);
		lseek(dosya_fd, 0, SEEK_SET); // Dosyanin basina don
		// printf("dosya boy: %d\n",dosya_boyutu);

		dosya_icerik = (char *)malloc(dosya_boyutu + 1);
		if (dosya_icerik == NULL)
		{
			printf("Bellek tahsisi hatasi");
			close(dosya_fd);
			return (1);
		}

		// Dosya okuma islemi
		while (okunan_byte_sayisi < dosya_boyutu)
		{
			ssize_t okunan = read(dosya_fd, &buffer, 1);
			if (okunan == -1) // read hata durumunda -1 doner
			{
				perror("Dosya okuma hatasi\n");
				close(dosya_fd);
				free(dosya_icerik);
				return (-1);
			}
			dosya_icerik[okunan_byte_sayisi] = buffer;
			if (buffer == '\n')
			{
				newline_sayisi++;
			}
			okunan_byte_sayisi++;
		}
		dosya_icerik[okunan_byte_sayisi] = '\0';
		close(dosya_fd);

		return_value = strdup(dosya_icerik);
		// printf("Dosya Icerigi: %s\n", return_value);

		if (newline_sayisi == 0)
		{
			return_value_size = 1;
		}
		else if (newline_sayisi > 0)
		{
			return_value_size = newline_sayisi + 1; // ustteki olay gibi
		}

		free(dosya_icerik);
	}
	else
	{
		printf("Hatali mod girisi\n");
		return 0;
	}
	// printf("%s\n",return_value);
	// printf("%d\n",return_value_size);

	// tek satirli global return_value \n lardan ayrılarak iki boyutlu hala getiriliyor

	char **ret = (char **)malloc(sizeof(char *) * (return_value_size + 1)); // aldigimiz tek boyutlu girdiyi strtok ile iki boyutlu hale getiriyoruz
	if(ret == NULL)
	{
		printf("malloc error\n");
		exit(1);
	}
	char *str = strtok(return_value, "\n");
	int i = 0;
	while (str != NULL)
	{
		ret[i] = (char *)malloc(sizeof(char) * (strlen(str) + 1));
		if(ret[i] == NULL)
		{
			printf("malloc error\n");
			exit(1);
		}
		strcpy(ret[i], str);
		i++;
		str = strtok(NULL, "\n");
	}
	ret[i] = NULL;
	// print_array(ret);

	int girilecek_satir_sayisi = 0;
	printf("Datadan kac farkli satir secilecek : ");
	scanf("%d", &girilecek_satir_sayisi);
	if ((girilecek_satir_sayisi > return_value_size) || girilecek_satir_sayisi <= 0)
	{
		printf("Yanlis farkli satir numarasi girisi\n");
		return 0;
	}

	printf("Secilecek satirlar : \n");
	int *girilen_satirlar = (int *)malloc(sizeof(int) * (girilecek_satir_sayisi + 1));
	if(girilen_satirlar == NULL)
	{
		printf("malloc error\n");
		exit(1);
	}
	for (int i = 0; i < girilecek_satir_sayisi; i++)
	{
		girilen_satirlar[i] = -1; // -1 ile dolduruyorum
	}
	for (int i = 0; i < girilecek_satir_sayisi; i++)
	{
		printf("%d. satir no : ", i + 1);
		scanf("%d", &girilen_satirlar[i]);
		if (girilen_satirlar[i] > return_value_size || girilen_satirlar[i] <= 0)
		{
			printf("Yanlis satir sayisi girisi\n");
			return 0;
		}
		if (girilecek_satir_sayisi > 1) // daha �nce girilmis mi kontrol�
		{
			for (int j = 0; j < i; j++)
			{
				if (girilen_satirlar[i] == girilen_satirlar[j])
				{
					printf("Yanlis giris - Bu karakter daha once girilmis!\n");
					return 0;
				}
			}
		}
	}
	girilen_satirlar[girilecek_satir_sayisi] = -1; // +1'lik kisim null atilamadigindan b�yle yaptim
	// print_array2(girilen_satirlar);

	printf("Secilmesi istenilen satir sayilari sorunsuz bir sekilde alindi!\n");

	double birim_sondaj_maaliyeti;
	printf("Birim sondaj maaliyetini giriniz: ");
	scanf("%lf", &birim_sondaj_maaliyeti);
	// printf("birim_sondaj: %f\n",birim_sondaj_maaliyeti);
	if (!(birim_sondaj_maaliyeti >= 1 && birim_sondaj_maaliyeti <= 10)) // Birim sondaj maliyeti ise kullan�c� taraf�ndan 1 ile 10 aras�nda de�er alabilecek bir de�i�ken parametre olmal�d�r
	{
		printf("Hatali birim sondaj maaliyeti girisi\n");
		return (0);
	}

	double birim_platform_maaliyeti;
	printf("Birim platform maaliyetini giriniz: ");
	scanf("%lf", &birim_platform_maaliyeti);

	int l = 0;
	struct satir *satirList = NULL;
	while (l < girilecek_satir_sayisi)
	{
		struct satir *newSatir = (struct satir *)malloc(sizeof(struct satir));
		if(newSatir == NULL)
		{
			printf("malloc error\n");
			exit(1);
		}
		newSatir->nokta_listesi = NULL;
		newSatir->satir_no = girilen_satirlar[l] - 1;
		// printf("niye la - > %s\n",ret[newSatir->satir_no]);
		if (ret[newSatir->satir_no][0] - '0' != newSatir->satir_no + 1 || ret[newSatir->satir_no][1] != 'B' || ret[newSatir->satir_no][strlen(ret[newSatir->satir_no]) - 1] != 'F') // baslangic sayi - sonraki B ve sonraki F kontrolu
		{
			printf("Hatali input syntax\n");
			return 0;
		}
		newSatir->satir_data = (char *)malloc(sizeof(char) * (strlen(ret[newSatir->satir_no]) - 2)); // scanf ile alınan veri - 2 kadar yer. -2 baslangıc ve sonu almama ve null yani -> -3 + 1 = -2
		if(newSatir->satir_data == NULL)
		{
			printf("malloc error\n");
			exit(1);
		}
		int baslangic1 = 0;
		for (int i = 2; i < strlen(ret[newSatir->satir_no]) - 1; i++)
		{
			newSatir->satir_data[baslangic1] = ret[newSatir->satir_no][i];
			baslangic1++;
		}
		newSatir->satir_data[baslangic1] = '\0'; // sona null
		newSatir->c_satir_data = (char *)malloc(sizeof(char) * (strlen(newSatir->satir_data) + 1));
		if(newSatir->c_satir_data == NULL)
		{
			printf("malloc error\n");
			exit(1);
		}
		strcpy(newSatir->c_satir_data, newSatir->c_satir_data);
		;
		newSatir->sekil_sayisi = 0; // S�f�ra ba�lat
		newSatir->nokta_sayisi = count(newSatir->satir_data, '(');

		char *str2 = strtok(newSatir->satir_data, "(");
		char *str_x, *str_y;
		while (str2 != NULL)
		{
			struct nokta *newNokta = (struct nokta *)malloc(sizeof(struct nokta));
			if(newNokta == NULL)
			{
				printf("malloc error\n");
				exit(1);
			}
			int uzun1 = 0, uzun2 = 0, search = 0, flag = 0;
			while (str2[search] != '\0')
			{
				if (!flag && str2[search] != ',') // virgul oncesi uzunluk
					uzun1++;
				else if (str2[search] == ',') // virgul kontrolu
					flag = 1;
				else if (flag && str2[search] != ',') // virgul sonrasi uzunluk
					uzun2++;
				else if (flag && str2[search] == ',') // fazla , var mi kontrol
				{
					printf("Yanlis sekilde girdi tespit edildi!!\n");
					return (0);
				}
				search++;
			}
			if (flag != 1) // hic , yoksa
			{
				printf("Yanlis sekilde girdi tespit edildi!!\n");
				return (0);
			}
			// printf("%d %d\n",uzun1,uzun2);

			str_x = (char *)malloc(sizeof(char) * (uzun1 + 1));
			if(str_x == NULL)
			{
				printf("malloc error\n");
				exit(1);
			}
			str_y = (char *)malloc(sizeof(char) * (uzun2 + 1));
			if(str_y == NULL)
			{
				printf("malloc error\n");
				exit(1);
			}

			search = 0;
			while (str2[search] != ',') // ilk kısım str_x'e atildi
			{
				str_x[search] = str2[search];
				search++;
			}
			str_x[search] = '\0';
			if (str2[search] == ',') // virgul mu kontrol yoksa hata
				search++;
			else
			{
				printf("Yanlis sekilde girdi tespit edildi!!\n");
				return (0);
			}
			int str_y_index = 0;
			while (str2[search] != ')') // ikinci kisim str_y'ye atildi
			{
				str_y[str_y_index] = str2[search];
				str_y_index++;
				search++;
			}
			str_y[str_y_index] = '\0';

			newNokta->x = atof(str_x); // array to float
			newNokta->y = atof(str_y);

			newNokta->next = newSatir->nokta_listesi; // basa ekleme
			newSatir->nokta_listesi = newNokta;
			str2 = strtok(NULL, "(");
		}
		newSatir->next = satirList; // basa ekleme
		satirList = newSatir;
		l += 1;
	}

	// sekil sayisi bulucu
	int sekil_sayisi_ = 0;
	int sayici = 0;
	struct satir *tem1 = satirList;
	while (tem1 != NULL)
	{
		int sayici = 0;
		struct nokta *ic_tem1 = tem1->nokta_listesi;
		while (ic_tem1 != NULL)
		{
			struct nokta *ic_tem2 = ic_tem1->next;
			int end_flag = 0;
			while (ic_tem2 != NULL)
			{
				if (ic_tem1->x == ic_tem2->x && ic_tem1->y == ic_tem2->y) // aynı iki nokta varsa sekil var demek
				{
					sayici++;
					if (ic_tem2->next)
					{
						ic_tem1 = ic_tem2;
						break;
					}
					else
					{
						end_flag = 1;
						break;
					}
				}
				ic_tem2 = ic_tem2->next;
			}
			if (end_flag)
			{
				break;
			}
			ic_tem1 = ic_tem1->next;
		}
		// printf("**sekil sayisi:%d\n",sayici);
		tem1->sekil_sayisi = sayici;
		sekil_sayisi_ += sayici;
		tem1 = tem1->next;
	}

	struct satir *tem20 = satirList;
	while (tem20 != NULL)
	{
		if (tem20->sekil_sayisi > 0)
		{
			tem20->sekil_eleman_tutucu = (int *)malloc(sizeof(int) * (tem20->sekil_sayisi * 2 + 1)); // ayni noktalarin indexlerinin tutuldugu dizi
			if(tem20->sekil_eleman_tutucu == NULL)
			{
				printf("malloc error\n");
				exit(1);
			}
			int zero = 0;
			int nok_sayisi = tem20->nokta_sayisi;
			int sayan = 0;
			struct nokta *ic_tem10 = tem20->nokta_listesi;
			while (ic_tem10 != NULL)
			{
				struct nokta *ic_tem20 = ic_tem10->next;
				tem20->sekil_eleman_tutucu[zero] = nok_sayisi;
				zero++;
				nok_sayisi--; // burada bir noktaya giriyor - baslangici yazdi
				int end_flag = 0;
				while (ic_tem20 != NULL)
				{
					// printf("%f %f\n",ic_tem20->x,ic_tem20->y);
					if (ic_tem10->x == ic_tem20->x && ic_tem10->y == ic_tem20->y)
					{
						tem20->sekil_eleman_tutucu[zero] = nok_sayisi; // digerini yazdi
						zero++;
						if (ic_tem20->next) // daha eleman varsa en son bulunan yerden devam
						{
							ic_tem10 = ic_tem20;
							nok_sayisi--;
							break;
						}
						else // yoksa bitir
						{
							end_flag = 1;
							nok_sayisi--;
							break;
						}
					}
					nok_sayisi--;
					ic_tem20 = ic_tem20->next;
				}
				if (end_flag)
				{
					break;
				}
				ic_tem10 = ic_tem10->next;
			}
			tem20->sekil_eleman_tutucu[zero] = -1905; // en sona null degil bu
		}
		else
		{
			printf("Sekil sayisi(%d) hatali!\n", tem20->sekil_sayisi);
		}
		tem20 = tem20->next;
	}

	int wh, ww;
	wnd_array = (int **)malloc(window_height / 10 * sizeof(int *));
	if (wnd_array == NULL) // malloc kontrol�
	{
		printf("Bellek tahsis hatasi\n");
		return 0;
	}

	for (wh = 0; wh < window_height / 10; wh++)
	{
		wnd_array[wh] = (int *)malloc(window_width / 10 * sizeof(int));
		if (wnd_array[wh] == NULL)
		{
			printf("Bellek tahsis hatasi\n");
			return 0;
		}
		for (ww = 0; ww < window_width / 10; ww++)
		{
			wnd_array[wh][ww] = -1;
		}
	}

	// cizdirme basliyor!
	setcolor(WHITE);
	fill_square();
	struct satir *tem2 = satirList;
	print_dots(tem2);

	// cizgi cizme
	struct satir *tem3 = satirList;
	print_lines(tem3);


	struct satir *tem4 = satirList;
	cizgi_karelerini_doldurma(tem4, 7);

	struct satir *tem5 = satirList;
	int toplam_sekil_sayisi = sekil_sayisi_hesaplama(tem5);

	// wnd_array_copy2
	wnd_array_copy2 = (int **)malloc(window_height / 10 * sizeof(int *));
	if (wnd_array_copy2 == NULL) // malloc kontrol�
	{
		printf("Bellek tahsis hatasi\n");
		return 0;
	}
	for (wh = 0; wh < window_height / 10; wh++)
	{
		wnd_array_copy2[wh] = (int *)malloc(window_width / 10 * sizeof(int));
		if (wnd_array_copy2[wh] == NULL)
		{
			printf("Bellek tahsis hatasi\n");
			return 0;
		}
		for (ww = 0; ww < window_width / 10; ww++)
		{
			wnd_array_copy2[wh][ww] = wnd_array[wh][ww];
		}
	}
	kare_iclerini_doldurma(toplam_sekil_sayisi, 7);

	// tekrardan
	tem2 = satirList;
	print_dots(tem2);
	tem3 = satirList;
	print_lines(tem3);

	int *sekil_alan_dizisi = (int *)malloc(sizeof(int) * (toplam_sekil_sayisi + 1));
	if(sekil_alan_dizisi == NULL)
	{
		printf("malloc error\n");
		exit(1);
	}
	int a_ = 0;
	while (a_ < toplam_sekil_sayisi)
	{
		sekil_alan_dizisi[a_] = wnd_count(a_ + 1);
		// printf("%d ",sekil_alan_dizisi[a_]);
		a_++;
	}
	sekil_alan_dizisi[a_] = -1;

	// alan yazdirma
	a_ = 0;
	while (a_ < toplam_sekil_sayisi)
	{
		int satir_sayisi = window_height / 10;
		int sutun_sayisi = window_width / 10;
		int bas_x = window_width / 10;
		int son_x = 0;
		int bas_y = window_height / 10;
		int son_y = 0;
		int y_ = 0;
		while (y_ < satir_sayisi)
		{
			int x_ = 0;
			while (x_ < sutun_sayisi)
			{

				if (wnd_array[y_][x_] == a_ + 1)
				{
					if (bas_x > x_)
					{
						bas_x = x_;
					}
					if (bas_y > y_)
					{
						bas_y = y_;
					}

					if (son_x < x_)
					{
						son_x = x_;
					}
					if (son_y < y_)
					{
						son_y = y_;
					}
				}
				x_++;
			}
			y_++;
		}

		// seklin ortasina alan yazdirma islemi
		settextstyle(2, HORIZ_DIR, 5); // HORIZ_DIRECTION : Yatay
		setcolor(MAGENTA);
		int yazdirilacak_x = (bas_x + son_x) / 2; // ,l� k�s�m� atmak icin
		int yazdirilacak_y = (bas_y + son_y) / 2;

		char charTemp1[50];
		sprintf(charTemp1, "%d", sekil_alan_dizisi[a_]);
		char *combined = (char *)malloc(sizeof(char) * (strlen(charTemp1) + 5)); // br^2 + \0
		if(combined == NULL)
		{
			printf("malloc error\n");
			exit(1);
		}
		combined = strjoin(charTemp1, "br^2");
		// printf("**********%s\n",combined);

		outtextxy(yazdirilacak_x * 10, yazdirilacak_y * 10, combined);

		a_++;
	}

	// rezerv hesaplama
	int *rezerv_deger_dizisi = (int *)malloc(sizeof(int) * (toplam_sekil_sayisi + 1));
	if(rezerv_deger_dizisi == NULL)
	{
		printf("malloc error\n");
		exit(1);
	}
	a_ = 0;
	while (a_ < toplam_sekil_sayisi)
	{
		rezerv_deger_dizisi[a_] = sekil_alan_dizisi[a_] * 10;
		a_++;
	}
	rezerv_deger_dizisi[a_] = -1;

	// rezerv de�eri yazd�rma

	a_ = 0;
	int rezerv_toplami = 0;
	while (rezerv_deger_dizisi[a_] != -1)
	{
		rezerv_toplami += rezerv_deger_dizisi[a_];
		a_++;
	}
	delay(1000);
	printf("Toplam rezerv degeri -> %d\n", rezerv_toplami);
	int secim;
	printf("2. asamaya gecmek icin 1 giriniz. Farkli bir karakter girdiginizde program sonlanacaktir!\n");
	printf("Secim :");
	scanf("%d", &secim);

	if (secim == 1)
	{
		printf("Ilk pencere kapatilip 2. pencere aciliyor!\n");
		closegraph();
	}
	else
	{
		printf("Yanlis giris yapildi\n");
		return (0);
	}

	initwindow(window_width, window_height);
	
	
	settextstyle(1, HORIZ_DIR, 5);
	setcolor(WHITE);
	fill_square();
	struct satir *tem6 = satirList;
	print_dots(tem6);
	struct satir *tem7 = satirList;
	print_lines(tem7);
	setcolor(WHITE);

	int satir_sayisi_ = window_height / 10;
	int sutun_sayisi_ = window_width / 10;
	int a = 0;
	while (a < satir_sayisi_)
	{
		int b = 0;
		while (b < sutun_sayisi_)
		{
			if (wnd_array[a][b] != -1)
			{
				// printf("%d\n",wnd_array[a][b]);
				int x_ = b * 10;
				int y_ = a * 10;
				rectangle(x_, y_, x_ + 10, y_ + 10);
				setfillstyle(SOLID_FILL, LIGHTGRAY); // Kareyi doldurmak i�in rengi ayarla
				floodfill((x_ + x_ + 10) / 2, (y_ + y_ + 10) / 2, WHITE);
			}
			b++;
		}
		a++;
	}

	tem6 = satirList;
	print_dots(tem6);
	setcolor(10);
	tem7 = satirList;
	print_lines(tem7);

	// wnd_array_copy
	wnd_array_copy = (int **)malloc(window_height / 10 * sizeof(int *));
	if (wnd_array_copy == NULL) // malloc kontrol�
	{
		printf("Bellek tahsis hatasi\n");
		return 0;
	}
	for (wh = 0; wh < window_height / 10; wh++)
	{
		wnd_array_copy[wh] = (int *)malloc(window_width / 10 * sizeof(int));
		if (wnd_array_copy[wh] == NULL)
		{
			printf("Bellek tahsis hatasi\n");
			return 0;
		}
		for (ww = 0; ww < window_width / 10; ww++)
		{
			wnd_array_copy[wh][ww] = wnd_array[wh][ww];
		}
	}

	// wnd_array_coppy_real
	wnd_array_copy_real = (int **)malloc(window_height / 10 * sizeof(int *));
	if (wnd_array_copy_real == NULL) // malloc kontrol�
	{
		printf("Bellek tahsis hatasi\n");
		return 0;
	}
	for (wh = 0; wh < window_height / 10; wh++)
	{
		wnd_array_copy_real[wh] = (int *)malloc(window_width / 10 * sizeof(int));
		if (wnd_array_copy_real[wh] == NULL)
		{
			printf("Bellek tahsis hatasi\n");
			return 0;
		}
		for (ww = 0; ww < window_width / 10; ww++)
		{
			wnd_array_copy_real[wh][ww] = wnd_array[wh][ww];
		}
	}

	// printf("***sekil sayisi -> %d \n",sekil_sayisi_);
	// altta kullan�yorum break icin

	// minimum say�da kare - yani platformun paras� �ok �ok fazla olmasi laz�m - a durumu
	a_ = 0;
	int toplam_alan_min_kare = 0;
	int sekil_sayisi_min_kare = 0;

	int temp_toplam_alan_min_kare = 0;
	int temp_sekil_sayisi_min_kare = 0;

	while (a_ < toplam_sekil_sayisi)
	{
		int bas_x = window_width / 10;
		int son_x = 0;
		int bas_y = window_height / 10;
		int son_y = 0;
		int satir_sayisi = window_height / 10;
		int sutun_sayisi = window_width / 10;

		int y_ = 0;
		while (y_ < satir_sayisi)
		{
			int x_ = 0;
			while (x_ < sutun_sayisi)
			{
				if (wnd_array_copy[y_][x_] == a_ + 1)
				{
					if (bas_x > x_)
					{
						bas_x = x_;
					}
					if (bas_y > y_)
					{
						bas_y = y_;
					}

					if (son_x < x_)
					{
						son_x = x_;
					}
					if (son_y < y_)
					{
						son_y = y_;
					}
				}
				x_++;
			}
			y_++;
		}
		int temp_bas_x = bas_x;
		int temp_bas_y = bas_y;
		// printf("bas -> %d %d\n",bas_x,bas_y);
		int x_fark = son_x - bas_x;
		int y_fark = son_y - bas_y;
		// printf("fark : %d %d\n",x_fark,y_fark);

		int baslanacak_yer = 16;
		/*
		if (x_fark < 8 || y_fark < 8)
			baslanacak_yer = 16;
		else if (x_fark > 4 || y_fark > 4)
			baslanacak_yer = 8;
		else if (x_fark > 2 || y_fark > 2)
			baslanacak_yer = 4;
		else if (x_fark > 1 || y_fark > 1)
			baslanacak_yer = 2;
		else
			baslanacak_yer = 1;
		*/
		if(x_fark <= 1 && y_fark <= 1)
		{
			baslanacak_yer = 1;
		}
		else if(x_fark <= 2 && y_fark <= 2)
		{
			baslanacak_yer = 2;
		}
		else if(x_fark <= 4 && y_fark <= 4)
		{
			baslanacak_yer = 4;
		}
		else if(x_fark <= 8 && y_fark <= 8)
		{
			baslanacak_yer = 8;
		}
		
		// printf("%d %d %d %d\n",bas_x,bas_y,son_x,son_y);
		// setcolor(10);
		int onceki_cizilen = -999; // baslangic
		int hata_flagi = 0;
		int sekil = 0;
		while (wnd_copy_count(a_ + 1) >= 0) // 64 - 16 - 4  - 1
		{
			if (wnd_copy_count(a_ + 1) == 0 && hata_flagi == 0)
			{
				//printf("Bittiiii!\n");
				//printf("sekil sayisi: %d\n", sekil_sayisi_min_kare);
				//printf("alan : %d\n", toplam_alan_min_kare);
				break;
			}

			if (hata_flagi == 1)
			{
				if (baslanacak_yer == 16)
					baslanacak_yer = 8;
				else if (baslanacak_yer == 8)
					baslanacak_yer = 4;
				else if (baslanacak_yer == 4)
					baslanacak_yer = 2;
				else if (baslanacak_yer == 2)
					baslanacak_yer = 1;

				// onceki kareleri temizleme
				int y_gezen = temp_bas_y;
				int x_gezen = temp_bas_x;
				// printf("test: %d\n",wnd_array_copy[y_gezen][temp_bas_x]);

				// printf("%d - %d \n",son_x,son_y);
				for (int y5 = 0; y5 < window_height/10; y5++)
				{
					for (int x5 = 0; x5 < window_width/10; x5++)
					{
						if (wnd_array_copy[y5][x5] >= (100 + 100 * (a_)) && wnd_array_copy[y5][x5] <= (104 + 100 * (a_)))
						{
							// printf("hob : %d %d\n",y_gezen,x_gezen);
							kare_duzeltme(x5 * 10, y5 * 10);
						}
					}
				}

				for (wh = 0; wh < window_height / 10; wh++)
				{
					for (ww = 0; ww < window_width / 10; ww++)
					{
						wnd_array_copy[wh][ww] = wnd_array_copy_real[wh][ww];
					}
				}
				toplam_alan_min_kare = temp_toplam_alan_min_kare;
				sekil_sayisi_min_kare = temp_sekil_sayisi_min_kare;

				hata_flagi = 0;
				onceki_cizilen = -999;
				bas_x = temp_bas_x;
				bas_y = temp_bas_y;
			}

			// printf("%d --- %d \n",bas_x, bas_y);
			if (baslanacak_yer == 16)
			{
				int onalti = on_altilik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
				if (onalti != -1)
				{
					//printf("16 cizildi %d %d!\n",bas_x,bas_y);

					if (onceki_cizilen != -999 && onceki_cizilen != 16)
					{
						//printf("HATA!\n");
						hata_flagi = 1;
						continue;
					}

					sekil = 16;
					baslanacak_yer = 16;
					toplam_alan_min_kare += 16 * 16;
					sekil_sayisi_min_kare += 1;
					onceki_cizilen = 16;
				}
				else
				{
					int sekiz = sekizlik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
					if (sekiz != -1)
					{
						//printf("8 cizildi!\n");
						if (onceki_cizilen != -999 && onceki_cizilen != 16 && onceki_cizilen != 8)
						{
							//printf("HATA!\n");
							hata_flagi = 1;
							continue;
						}
						sekil = 8;
						baslanacak_yer = 8;
						toplam_alan_min_kare += 8 * 8;
						sekil_sayisi_min_kare += 1;
						onceki_cizilen = 8;
					}
					else
					{
						int dort = dortluk_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
						if (dort != -1)
						{

							//printf("4 cizildi!\n");
							if (onceki_cizilen != -999 && onceki_cizilen != 8 && onceki_cizilen != 4)
							{
								//printf("HATA!\n");
								hata_flagi = 1;
								continue;
							}
							sekil = 4;
							baslanacak_yer = 4;
							toplam_alan_min_kare += 4 * 4;
							sekil_sayisi_min_kare += 1;
							onceki_cizilen = 4;
						}
						else
						{
							int iki = ikilik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
							if (iki != -1)
							{
								//printf("2 cizildi!\n");
								if (onceki_cizilen != -999 && onceki_cizilen != 4 && onceki_cizilen != 2)
								{
									//printf("HATA!\n");
									hata_flagi = 1;
									continue;
								}
								sekil = 2;
								baslanacak_yer = 2;
								toplam_alan_min_kare += 2 * 2;
								sekil_sayisi_min_kare += 1;
								onceki_cizilen = 2;
							}
							else
							{
								if (onceki_cizilen != -999 && onceki_cizilen != 2 && onceki_cizilen != 1)
								{
									//printf("HATA!\n");
									hata_flagi = 1;
									continue;
								}
								birlik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
								//printf("1 cizildi\n");
								sekil = 1;
								baslanacak_yer = 1;
								toplam_alan_min_kare += 1 * 1;
								sekil_sayisi_min_kare += 1;
								onceki_cizilen = 1;
							}
						}
					}
				}
			}
			else if (baslanacak_yer == 8)
			{
				int sekiz = sekizlik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
				if (sekiz != -1)
				{
					//printf("8 cizildi!\n");
					if (onceki_cizilen != -999 && onceki_cizilen != 16 && onceki_cizilen != 8)
					{
						//printf("HATA!\n");
						hata_flagi = 1;
						continue;
					}
					sekil = 8;
					baslanacak_yer = 8;
					toplam_alan_min_kare += 8 * 8;
					sekil_sayisi_min_kare += 1;
					onceki_cizilen = 8;
				}
				else
				{
					int dort = dortluk_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
					if (dort != -1)
					{
						//printf("4 cizildi!\n");
						if (onceki_cizilen != -999 && onceki_cizilen != 8 && onceki_cizilen != 4)
						{
							//printf("HATA!\n");
							hata_flagi = 1;
							continue;
						}
						sekil = 4;
						baslanacak_yer = 4;
						toplam_alan_min_kare += 4 * 4;
						sekil_sayisi_min_kare += 1;
						onceki_cizilen = 4;
					}
					else
					{
						int iki = ikilik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
						if (iki != -1)
						{
							//printf("2 cizildi!\n");
							if (onceki_cizilen != -999 && onceki_cizilen != 4 && onceki_cizilen != 2)
							{
								//printf("HATA!\n");
								hata_flagi = 1;
								continue;
							}
							sekil = 2;
							baslanacak_yer = 2;
							toplam_alan_min_kare += 2 * 2;
							sekil_sayisi_min_kare += 1;
							onceki_cizilen = 2;
						}
						else
						{
							if (onceki_cizilen != -999 && onceki_cizilen != 2 && onceki_cizilen != 1)
							{
								//printf("HATA!\n");
								hata_flagi = 1;
								continue;
							}
							birlik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
							//printf("1 cizildi\n");
							sekil = 1;
							baslanacak_yer = 1;
							toplam_alan_min_kare += 1 * 1;
							sekil_sayisi_min_kare += 1;
							onceki_cizilen = 1;
						}
					}
				}
			}
			else if (baslanacak_yer == 4)
			{
				int dort = dortluk_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
				if (dort != -1)
				{
					//printf("4 cizildi!\n");
					if (onceki_cizilen != -999 && onceki_cizilen != 8 && onceki_cizilen != 4)
					{
						//printf("HATA!\n");
						hata_flagi = 1;
						continue;
					}
					sekil = 4;
					baslanacak_yer = 4;
					toplam_alan_min_kare += 4 * 4;
					sekil_sayisi_min_kare += 1;
					onceki_cizilen = 4;
				}
				else
				{
					int iki = ikilik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
					if (iki != -1)
					{
						//printf("2 cizildi!\n");
						if (onceki_cizilen != -999 && onceki_cizilen != 4 && onceki_cizilen != 2)
						{
							//printf("HATA!\n");
							hata_flagi = 1;
							continue;
						}
						sekil = 2;
						baslanacak_yer = 2;
						toplam_alan_min_kare += 2 * 2;
						sekil_sayisi_min_kare += 1;
						onceki_cizilen = 2;
					}
					else
					{
						if (onceki_cizilen != -999 && onceki_cizilen != 2 && onceki_cizilen != 1)
						{
							//printf("HATA!\n");
							hata_flagi = 1;
							continue;
						}
						birlik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
						//printf("1 cizildi\n");
						sekil = 1;
						baslanacak_yer = 1;
						toplam_alan_min_kare += 1 * 1;
						sekil_sayisi_min_kare += 1;
						onceki_cizilen = 1;
					}
				}
			}
			else if (baslanacak_yer == 2)
			{
				int iki = ikilik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
				if (iki != -1)
				{
					//printf("2 cizildi!\n");
					if (onceki_cizilen != -999 && onceki_cizilen != 4 && onceki_cizilen != 2)
					{
						//printf("HATA!\n");
						hata_flagi = 1;
						continue;
					}
					sekil = 2;
					baslanacak_yer = 2;
					toplam_alan_min_kare += 2 * 2;
					sekil_sayisi_min_kare += 1;
					onceki_cizilen = 2;
				}
				else
				{
					if (onceki_cizilen != -999 && onceki_cizilen != 2 && onceki_cizilen != 1)
					{
						//printf("HATA!\n");
						hata_flagi = 1;
						continue;
					}
					birlik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
					//printf("1 cizildi\n");
					sekil = 1;
					baslanacak_yer = 1;
					toplam_alan_min_kare += 1 * 1;
					sekil_sayisi_min_kare += 1;
					onceki_cizilen = 1;
				}
			}
			else
			{
				if (onceki_cizilen != -999 && onceki_cizilen != 2 && onceki_cizilen != 1)
				{
					//printf("HATA!\n");
					hata_flagi = 1;
					continue;
				}
				birlik_ciz(bas_x, bas_y, son_x, son_y, a_ + 1);
				// printf("1 cizildi\n");
				sekil = 1;
				baslanacak_yer = 1;
				toplam_alan_min_kare += 1 * 1;
				sekil_sayisi_min_kare += 1;
				onceki_cizilen = 1;
			}
			int onceki_bas_x = bas_x;
			int onceki_bas_y = bas_y;
			bas_x = window_width / 10;
			bas_y = window_height / 10;

			int y_min = window_height / 10;

			int y1_ = 0;
			while (y1_ < satir_sayisi)
			{
				int x1_ = 0;
				while (x1_ < sutun_sayisi)
				{
					if (wnd_array_copy[y1_][x1_] == a_ + 1)
					{

						if (bas_x > x1_) // y en kucuk nerede ise ona gore
						{
							// printf("uc -> %d %d %d\n",y1_,x1_,wnd_array_copy[y1_][x1_]);
							bas_x = x1_;
							if (onceki_bas_x == bas_x)
								bas_y = onceki_bas_y + sekil;
							else
							{
								bas_y = y1_;
							}
						}

						if (y_min > y1_)
						{
							y_min = y1_;
						}
					}
					x1_++;
				}
				y1_++;
			}
			if (onceki_bas_x == bas_x)
			{
				bas_x = onceki_bas_x;
				bas_y = onceki_bas_y + sekil;
			}

			// printf("%d %d\n",y_min,bas_x);
			if (y_min != window_height / 10 && wnd_array_copy[y_min][bas_x] == -1 && bas_x == onceki_bas_x + sekil)
				bas_y = y_min;

			//printf("%d %d\n", bas_y, bas_x);
		}
		temp_toplam_alan_min_kare = toplam_alan_min_kare;
		temp_sekil_sayisi_min_kare = sekil_sayisi_min_kare;
		if (wnd_copy_count(a_ + 1) == 0)
		{
			for (wh = 0; wh < window_height / 10; wh++)
			{
				for (ww = 0; ww < window_width / 10; ww++)
				{
					wnd_array_copy_real[wh][ww] = wnd_array_copy[wh][ww];
				}
			}
		}

		a_++;
	}

	tem6 = satirList;
	print_dots(tem6);

	tem7 = satirList;
	print_lines(tem7);

	// min kare sonuc
	double min_kare_maaliyet = toplam_alan_min_kare * birim_sondaj_maaliyeti + sekil_sayisi_min_kare * birim_platform_maaliyeti;
	//printf("Min kare maaliyeti -> %lf\n", min_kare_maaliyet);

	int secim_ = 0;
	printf("Min kare sayisi tamamlandi devam etmek icin 1 girin. (1 harici girislerde program sonlanir!)\n");
	printf("Secim: ");
	scanf("%d", &secim_);
	if (secim_ == 1)
	{
		printf("1 girildi! Devam ediliyor...\n");
	}
	else
	{
		printf("1 harici giris yapildi. Program sona erdiriliyor...\n");
		return (0);
	}

	pencere_temizleme();

	delay(500);

	// min toplam alan

	int toplam_alan_min_toplam_alan = 0;
	int kare_sayisi_min_toplam_alan = 0;

	settextstyle(1, HORIZ_DIR, 5);
	setcolor(WHITE);
	fill_square();
	tem6 = satirList;
	print_dots(tem6);
	tem7 = satirList;
	print_lines(tem7);
	setcolor(WHITE);

	satir_sayisi_ = window_height / 10;
	sutun_sayisi_ = window_width / 10;
	a = 0;
	while (a < satir_sayisi_)
	{
		int b = 0;
		while (b < sutun_sayisi_)
		{
			if (wnd_array[a][b] != -1)
			{
				// printf("%d\n",wnd_array[a][b]);
				int x_ = b * 10;
				int y_ = a * 10;
				rectangle(x_, y_, x_ + 10, y_ + 10);
				setfillstyle(SOLID_FILL, 14); // Kareyi doldurmak i�in rengi ayarla
				floodfill((x_ + x_ + 10) / 2, (y_ + y_ + 10) / 2, WHITE);

				toplam_alan_min_toplam_alan += 1;
				kare_sayisi_min_toplam_alan += 1;
			}
			b++;
		}
		a++;
	}

	tem6 = satirList;
	print_dots(tem6);
	setcolor(10);
	tem7 = satirList;
	print_lines(tem7);
	double min_toplam_alan_maaliyet = toplam_alan_min_toplam_alan * birim_sondaj_maaliyeti + kare_sayisi_min_toplam_alan * birim_platform_maaliyeti;
	// printf("****%d %d\n",toplam_alan_min_toplam_alan,kare_sayisi_min_toplam_alan);
	//printf("Minimum toplam alan durumu maaliyeti = %lf\n", min_toplam_alan_maaliyet);

	int secim_2 = 0;
	printf("Min kare sayisi tamamlandi devam etmek icin 1 girin. (1 harici girislerde program sonlanir!)\n");
	printf("Secim: ");
	scanf("%d", &secim_2);
	if (secim_2 == 1)
	{
		printf("1 girildi! Devam ediliyor...\n");
	}
	else
	{
		printf("1 harici giris yapildi. Program sona erdiriliyor...\n");
		return (0);
	}

	pencere_temizleme();
	delay(500);

	//d buradan devam

	settextstyle(1, HORIZ_DIR, 5);
	setcolor(WHITE);
	fill_square();
	tem6 = satirList;
	print_dots(tem6);
	tem7 = satirList;
	print_lines2(tem7);
	setcolor(WHITE);

	int toplam_alan_optimize = 0;
	int sekil_sayisi_optimize = 0;
	satir_sayisi_ = window_height / 10;
	sutun_sayisi_ = window_width / 10;
	a = 0;
	while (a < satir_sayisi_)
	{
		int b = 0;
		while (b < sutun_sayisi_)
		{
			if (wnd_array[a][b] != -1)
			{
				int x_ = b * 10;
				int y_ = a * 10;
				rectangle(x_, y_, x_ + 10, y_ + 10);
				setfillstyle(SOLID_FILL, LIGHTGRAY); // Kareyi doldurmak i�in rengi ayarla
				floodfill((x_ + x_ + 10) / 2, (y_ + y_ + 10) / 2, WHITE);
				toplam_alan_optimize += 1;
				sekil_sayisi_optimize += 1;
			}
			b++;
		}
		a++;
	}

	tem6 = satirList;
	print_dots(tem6);
	setcolor(10);
	tem7 = satirList;
	print_lines2(tem7);
	struct satir *s_ = satirList;
	while (s_ != NULL)
	{
		int nokta_no = s_->nokta_sayisi;
		// printf("nokta no -> %d\n",nokta_no);
		int *nokta_dizisi = (int *)malloc(sizeof(int) * (nokta_no * 2));
		if(nokta_dizisi == NULL)
		{
			printf("malloc error\n");
			exit(1);
		}
		struct nokta *nok = s_->nokta_listesi;
		int idx = 0;
		while (nok != NULL)
		{

			nokta_dizisi[idx] = (int)round(nok->x * 10);
			nokta_dizisi[idx + 1] = (int)round(nok->y * 10);
			nok = nok->next;
			idx += 2;
		}

		setfillstyle(SOLID_FILL, LIGHTRED);
		fillpoly(nokta_no, nokta_dizisi);
		s_ = s_->next;
	}
	// LIGHTRED GORDUGUN YERE OPTIMIZE ARRAYINI AYARLA
	// wnd_array_optimize
	wnd_array_optimize = (int **)malloc(window_height * sizeof(int *));
	if (wnd_array_optimize == NULL) // malloc kontrol�
	{
		printf("Bellek tahsis hatasi\n");
		return 0;
	}
	for (wh = 0; wh < window_height; wh++)
	{
		wnd_array_optimize[wh] = (int *)malloc(window_width * sizeof(int));
		if (wnd_array_optimize[wh] == NULL)
		{
			printf("Bellek tahsis hatasi\n");
			return 0;
		}
		for (ww = 0; ww < window_width; ww++)
		{
			wnd_array_optimize[wh][ww] = -1;
		}
	}
	int sayac = 0;
	int y_ = 0;
	while (y_ < window_height)
	{
		int x_ = 0;
		while (x_ < window_width)
		{
			int renk = getpixel(x_, y_);
			if (renk == 12)
			{
				wnd_array_optimize[y_][x_] = 12;
				sayac++;
			}
			x_++;
		}
		y_++;
	}
	// printf("Sayac -> %d\n", sayac);
	for (wh = 0; wh < window_height / 10; wh++)
	{
		for (ww = 0; ww < window_width / 10; ww++)
		{
			wnd_array[wh][ww] = wnd_array_copy2[wh][ww];
		}
	}
	struct satir *strList = satirList;
	cizgi_karelerini_doldurma(strList, 14);
	kare_iclerini_doldurma(toplam_sekil_sayisi, 14);
	tem6 = satirList;
	print_dots(tem6);
	setcolor(10);
	tem7 = satirList;
	print_lines2(tem7);
	int sekil_no = 1;
	while (sekil_no <= toplam_sekil_sayisi)
	{
		// printf("sek no : %d\n", sekil_no);
		int bas_x = window_width / 10;
		int bas_y = window_height / 10;
		int son_x = 0;
		int son_y = 0;
		int y_ = 0;
		while (y_ < window_height / 10)
		{
			int x_ = 0;
			while (x_ < window_width / 10)
			{
				if (wnd_array[y_][x_] == sekil_no)
				{
					if (bas_x > x_)
					{
						bas_x = x_;
					}
					if (bas_y > y_)
					{
						bas_y = y_;
					}
					if (son_x < x_)
					{
						son_x = x_;
					}
					if (son_y < y_)
					{
						son_y = y_;
					}
				}
				x_++;
			}
			y_++;
		}
		// min x min y bulundu
		int temp_min_x = bas_x;
		int temp_min_y = bas_y;
		// max x max y
		int temp_max_x = son_x;
		int temp_max_y = son_y;
		// 2x2
		int sekil = 2;
		int l = temp_min_y;
		while (l < temp_max_y && l < window_height / 10)
		{
			int i = temp_min_x;
			while (i < temp_max_x && i < window_width / 10)
			{
				int i_temp = i;
				int l_temp = l;
				// min 2 kare o basamak olmasi lazim
				// sol ust(i,l)
				int sayan = 0;

				int cont_flag = 0;
				// 2x2 kontrolu
				for (int s = l; s < l + sekil; s++)
				{
					for (int t = i; t < i + sekil; t++)
					{
						if (wnd_array[s][t] == 103)
						{
							cont_flag = 1;
						}
					}
				}
				if (cont_flag == 1)
				{
					i++;
					continue;
				}
				for (int s = l; s < l + sekil; s++)
				{
					for (int t = i; t < i + sekil; t++)
					{
						if (wnd_array[s][t] == sekil_no)
						{

							sayan++;
						}
					}
				}
				int pixel_sayan = 0;
				if (sayan > sekil * sekil / 2) // min 2 kosulu saglandi - simdi opti pixel kontrol
				{
					for (int m = l * 10; m < l * 10 + sekil * 10; m++)
					{
						for (int k = i * 10; k < i * 10 + sekil * 10; k++)
						{
							if (wnd_array_optimize[m][k] == 12)
							{
								pixel_sayan++;
							}
						}
					}
				}
				if (pixel_sayan > (sekil * 10 * sekil * 10) / 2)
				{
					sekil_sayisi_optimize -= 3;
					for (int s = l_temp; s < l_temp + sekil; s++)
					{
						for (int t = i_temp; t < i_temp + sekil; t++)
						{
							wnd_array[s][t] = 103;
							kare_doldurma(t * 10, s * 10, 9);
						}
					}
					i += 1;
				}

				i += 1;
			}
			l += 1;
		}
		// 4x4
		sekil = 4;
		// printf("4x4\n");
		// printf("%d %d %d %d\n", temp_min_x, temp_min_y, temp_max_x, temp_max_y);
		l = temp_min_y;
		while (l < temp_max_y && l + 4 < window_height / 10)
		{
			int i = temp_min_x;
			while (i < temp_max_x && i + 4 < window_width / 10)
			{
				int l_temp = l;
				int i_temp = i;
				if (wnd_array[l][i] == 103)
				{
					int is4notprintable = 0;
					for (int s = l; s < l + 4; s++)
					{
						for (int t = i; t < i + 4; t++)
						{
							if (wnd_array[s][t] != 103)
							{
								is4notprintable = 1;
							}
						}
					}
					if (is4notprintable == 0) // simdi cevre kontrolu
					{
						int is4lines = 1;
						// ust
						// printf("ust\n");
						int m;
						int k;
						if (l_temp - 1 >= 0)
						{
							int m = l_temp - 1; // sol ust nokta kordinatlari - y
							int k = i_temp;		// x
							for (int z = k; z < k + 4; z++)
							{
								if (wnd_array[m][z] != -1 && wnd_array[m][z] != 103 && wnd_array[m][z] != 102)
								{
									is4lines = 0;
								}
								else
								{
									if (m - 1 > 0 && wnd_array[m - 1][z] != wnd_array[m][z])
									{
										is4lines = 0;
									}
								}
								int m_ = l_temp - 1;
								if (wnd_array[m_][z] == 103)
								{
									int sayici = 0;
									while (m_ > 0 && wnd_array[m_][z] == 103)
									{
										sayici++;
										m_--;
									}
									if (sayici % 2 == 1)
									{
										is4lines = 0;
									}
								}
							}
							if (is4lines == 0)
							{
								i += 2;
								continue;
							}
						}
						// alt
						// printf("alt\n");
						if (l_temp + 4 < window_height / 10)
						{
							int m = l_temp + 4;
							k = i_temp;
							for (int z = k; z < k + 4; z++)
							{
								if (wnd_array[m][z] != -1 && wnd_array[m][z] != 103 && wnd_array[m][z] != 102)
								{
									is4lines = 0;
								}
								else
								{
									if (m + 1 < window_height / 10 && wnd_array[m + 1][z] != wnd_array[m][z])
									{
										is4lines = 0;
									}
								}
								int m_ = l_temp + 4;

								if (wnd_array[m_][z] == 103)
								{
									int sayici = 0;
									while (m_ < window_height / 10 && wnd_array[m_][z] == 103)
									{
										sayici++;
										m_++;
									}
									if (sayici % 2 == 1) // 2 nin kati olmasi lazim
									{
										is4lines = 0;
									}
								}
							}
							if (is4lines == 0)
							{
								i += 2;
								continue;
							}
						}
						// sol kenar
						// printf("sol\n");
						if (i_temp - 1 >= 0)
						{
							m = l_temp;		// y
							k = i_temp - 1; // x
							for (int z = m; z < m + 4; z++)
							{
								if (wnd_array[z][k] != -1 && wnd_array[z][k] != 103 && wnd_array[z][k] != 102)
								{
									is4lines = 0;
								}
								else
								{
									if (k - 1 > 0 && wnd_array[z][k - 1] != wnd_array[z][k])
									{
										is4lines = 0;
									}
								}
								int k_ = i_temp - 1;

								if (wnd_array[z][k_] == 103)
								{
									int sayici = 0;
									while (k_ > 0 && wnd_array[z][k_] == 103)
									{
										sayici++;
										k_--;
									}
									if (sayici % 2 == 1) // 2 nin kati olmasi lazim
									{
										is4lines = 0;
									}
								}
							}
							if (is4lines == 0)
							{
								i += 2;
								continue;
							}
						}
						// sag kenar
						// printf("sag\n");
						if (i_temp + 4 < window_width / 10)
						{
							m = l_temp;
							k = i_temp + 4;
							for (int z = m; z < m + 4; z++)
							{
								if (wnd_array[z][k] != -1 && wnd_array[z][k] != 103 && wnd_array[z][k] != 102)
								{
									is4lines = 0;
								}
								else
								{
									if (k + 1 < window_width / 10 && wnd_array[z][k + 1] != wnd_array[z][k])
									{
										is4lines = 0;
									}
								}
								int k_ = i_temp + 4;
								if (wnd_array[z][k_] == 103)
								{
									int sayici = 0;
									while (k_ < window_height / 10 && wnd_array[z][k_] == 103)
									{
										sayici++;
										k_++;
									}
									if (sayici % 2 == 1) // 2 nin kati olmasi lazim
									{
										is4lines = 0;
									}
								}
							}
						}
						if (is4lines == 1)
						{
							// printf("4x4 cizilecek!!!\n");
							sekil_sayisi_optimize -= 3;
							for (int s = l_temp; s < l_temp + sekil; s++)
							{
								for (int t = i_temp; t < i_temp + sekil; t++)
								{
									wnd_array[s][t] = 102;
									kare_doldurma(t * 10, s * 10, 12);
								}
							}
						}
					}
					i++;
				}
				i++;
			}
			l++;
		}
		// 8x8
		sekil = 8;
		l = temp_min_y;
		while (l < temp_max_y && l + 8 < window_height / 10)
		{
			int i = temp_min_x;
			while (i < temp_max_x && i + 8 < window_width / 10)
			{
				int l_temp = l;
				int i_temp = i;
				if (wnd_array[l][i] == 102)
				{
					int is8notprintable = 0;
					for (int s = l; s < l + 8; s++)
					{
						for (int t = i; t < i + 8; t++)
						{
							if (wnd_array[s][t] != 102)
							{
								is8notprintable = 1;
							}
						}
					}
					if (is8notprintable == 0) // simdi cevre kontrolu
					{
						int is8lines = 1;
						// ust
						// printf("ust\n");
						int m;
						int k;
						if (l_temp - 1 >= 0)
						{
							int m = l_temp - 1; // sol ust nokta kordinatlari - y
							int k = i_temp;		// x
							for (int z = k; z < k + 8; z++)
							{
								if (wnd_array[m][z] != -1 && wnd_array[m][z] != 102 && wnd_array[m][z] != 101)
								{
									is8lines = 0;
								}
								else
								{
									if (m - 1 > 0 && wnd_array[m - 1][z] != wnd_array[m][z])
									{
										is8lines = 0;
									}
								}
								int m_ = l_temp - 1;
								if (wnd_array[m_][z] == 102)
								{
									int sayici = 0;
									while (m_ > 0 && wnd_array[m_][z] == 102)
									{
										sayici++;
										m_--;
									}
									if (sayici % 4 != 0)
									{
										is8lines = 0;
									}
								}
							}
							if (is8lines == 0)
							{
								i += 4;
								continue;
							}
						}
						// alt
						// printf("alt\n");
						if (l_temp + 8 < window_height / 10)
						{
							int m = l_temp + 8;
							k = i_temp;
							for (int z = k; z < k + 8; z++)
							{
								if (wnd_array[m][z] != -1 && wnd_array[m][z] != 102 && wnd_array[m][z] != 101)
								{
									is8lines = 0;
								}
								else
								{
									if (m + 1 < window_height / 10 && wnd_array[m + 1][z] != wnd_array[m][z])
									{
										is8lines = 0;
									}
								}
								int m_ = l_temp + 8;

								if (wnd_array[m_][z] == 102)
								{
									int sayici = 0;
									while (m_ < window_height / 10 && wnd_array[m_][z] == 102)
									{
										sayici++;
										m_++;
									}
									if (sayici % 4 != 0)
									{
										is8lines = 0;
									}
								}
							}
							if (is8lines == 0)
							{
								i += 4;
								continue;
							}
						}
						// sol kenar
						// printf("sol\n");
						if (i_temp - 1 >= 0)
						{
							m = l_temp;		// y
							k = i_temp - 1; // x
							for (int z = m; z < m + 8; z++)
							{
								if (wnd_array[z][k] != -1 && wnd_array[z][k] != 102 && wnd_array[z][k] != 101)
								{
									is8lines = 0;
								}
								else
								{
									if (k - 1 > 0 && wnd_array[z][k - 1] != wnd_array[z][k])
									{
										is8lines = 0;
									}
								}
								int k_ = i_temp - 1;

								if (wnd_array[z][k_] == 102)
								{
									int sayici = 0;
									while (k_ > 0 && wnd_array[z][k_] == 102)
									{
										sayici++;
										k_--;
									}
									if (sayici % 4 != 0)
									{
										is8lines = 0;
									}
								}
							}
							if (is8lines == 0)
							{
								i += 4;
								continue;
							}
						}
						// sag kenar
						// printf("sag\n");
						if (i_temp + 8 < window_width / 10)
						{
							m = l_temp;
							k = i_temp + 8;
							for (int z = m; z < m + 8; z++)
							{
								if (wnd_array[z][k] != -1 && wnd_array[z][k] != 102 && wnd_array[z][k] != 101)
								{
									is8lines = 0;
								}
								else
								{
									if (k + 1 < window_width / 10 && wnd_array[z][k + 1] != wnd_array[z][k])
									{
										is8lines = 0;
									}
								}
								int k_ = i_temp + 8;
								if (wnd_array[z][k_] == 102)
								{
									int sayici = 0;
									while (k_ < window_height / 10 && wnd_array[z][k_] == 102)
									{
										sayici++;
										k_++;
									}
									if (sayici % 4 != 0)
									{
										is8lines = 0;
									}
								}
							}
						}
						if (is8lines == 1)
						{
							// printf("8x8 cizilecek!!!\n");
							sekil_sayisi_optimize -= 3;
							for (int s = l_temp; s < l_temp + sekil; s++)
							{
								for (int t = i_temp; t < i_temp + sekil; t++)
								{
									wnd_array[s][t] = 101;
									kare_doldurma(t * 10, s * 10, 11);
								}
							}
						}
					}
					i += 3;
				}
				i++;
			}
			l++;
		}
		// 16x16
		sekil = 16;
		l = temp_min_y;
		while (l < temp_max_y && l + 16 < window_height / 10)
		{
			int i = temp_min_x;
			while (i < temp_max_x && i + 16 < window_width / 10)
			{
				int l_temp = l;
				int i_temp = i;
				if (wnd_array[l][i] == 101)
				{
					int is16notprintable = 0;
					for (int s = l; s < l + 16; s++)
					{
						for (int t = i; t < i + 16; t++)
						{
							if (wnd_array[s][t] != 101)
							{
								is16notprintable = 1;
							}
						}
					}
					if (is16notprintable == 0) // simdi cevre kontrolu
					{
						int is16lines = 1;
						// ust
						// printf("ust\n");
						int m;
						int k;
						if (l_temp - 1 >= 0)
						{
							int m = l_temp - 1; // sol ust nokta kordinatlari - y
							int k = i_temp;		// x
							for (int z = k; z < k + 16; z++)
							{
								if (wnd_array[m][z] != -1 && wnd_array[m][z] != 101 && wnd_array[m][z] != 100)
								{
									is16lines = 0;
								}
								else
								{
									if (m - 1 > 0 && wnd_array[m - 1][z] != wnd_array[m][z])
									{
										is16lines = 0;
									}
								}
								int m_ = l_temp - 1;
								if (wnd_array[m_][z] == 101)
								{
									int sayici = 0;
									while (m_ > 0 && wnd_array[m_][z] == 101)
									{
										sayici++;
										m_--;
									}
									if (sayici % 8 != 0)
									{
										is16lines = 0;
									}
								}
							}
							if (is16lines == 0)
							{
								i += 8;
								continue;
							}
						}
						// alt
						// printf("alt\n");
						if (l_temp + 16 < window_height / 10)
						{
							int m = l_temp + 16;
							k = i_temp;
							for (int z = k; z < k + 16; z++)
							{
								if (wnd_array[m][z] != -1 && wnd_array[m][z] != 101 && wnd_array[m][z] != 100)
								{
									is16lines = 0;
								}
								else
								{
									if (m + 1 < window_height / 10 && wnd_array[m + 1][z] != wnd_array[m][z])
									{
										is16lines = 0;
									}
								}
								int m_ = l_temp + 16;

								if (wnd_array[m_][z] == 101)
								{
									int sayici = 0;
									while (m_ < window_height / 10 && wnd_array[m_][z] == 101)
									{
										sayici++;
										m_++;
									}
									if (sayici % 8 != 0)
									{
										is16lines = 0;
									}
								}
							}
							if (is16lines == 0)
							{
								i += 8;
								continue;
							}
						}
						// sol kenar
						// printf("sol\n");
						if (i_temp - 1 >= 0)
						{
							m = l_temp;		// y
							k = i_temp - 1; // x
							for (int z = m; z < m + 16; z++)
							{
								if (wnd_array[z][k] != -1 && wnd_array[z][k] != 101 && wnd_array[z][k] != 100)
								{
									is16lines = 0;
								}
								else
								{
									if (k - 1 > 0 && wnd_array[z][k - 1] != wnd_array[z][k])
									{
										is16lines = 0;
									}
								}
								int k_ = i_temp - 1;

								if (wnd_array[z][k_] == 101)
								{
									int sayici = 0;
									while (k_ > 0 && wnd_array[z][k_] == 101)
									{
										sayici++;
										k_--;
									}
									if (sayici % 8 != 0)
									{
										is16lines = 0;
									}
								}
							}
							if (is16lines == 0)
							{
								i += 8;
								continue;
							}
						}
						// sag kenar
						// printf("sag\n");
						if (i_temp + 16 < window_width / 10)
						{
							m = l_temp;
							k = i_temp + 16;
							for (int z = m; z < m + 16; z++)
							{
								if (wnd_array[z][k] != -1 && wnd_array[z][k] != 101 && wnd_array[z][k] != 100)
								{
									is16lines = 0;
								}
								else
								{
									if (k + 1 < window_width / 10 && wnd_array[z][k + 1] != wnd_array[z][k])
									{
										is16lines = 0;
									}
								}
								int k_ = i_temp + 16;
								if (wnd_array[z][k_] == 101)
								{
									int sayici = 0;
									while (k_ < window_height / 10 && wnd_array[z][k_] == 101)
									{
										sayici++;
										k_++;
									}
									if (sayici % 8 != 0)
									{
										is16lines = 0;
									}
								}
							}
						}
						if (is16lines == 1)
						{
							// printf("16x16 cizilecek!!!\n");
							sekil_sayisi_optimize -= 3;
							for (int s = l_temp; s < l_temp + sekil; s++)
							{
								for (int t = i_temp; t < i_temp + sekil; t++)
								{
									wnd_array[s][t] = 100;
									kare_doldurma(t * 10, s * 10, 10);
								}
							}
						}
					}
					i += 7;
				}

				i++;
			}
			l++;
		}
		sekil_no++;
	}

	double optimize_maaliyet = toplam_alan_optimize * birim_sondaj_maaliyeti + sekil_sayisi_optimize * birim_platform_maaliyeti;
	//printf("optimize_maaliyet -> %lf\n", optimize_maaliyet);

	tem6 = satirList;
	print_dots(tem6);
	setcolor(10);
	tem7 = satirList;
	print_lines2(tem7);

	//void kare_duzeltme(double x, double y)
	
	int son_y = 0;
	while(son_y < window_height/10)
	{
		int son_x = 0;
		while(son_x < window_width/10)
		{
			if(wnd_array[son_y][son_x] == -1)
			{
				kare_duzeltme(son_x*10,son_y*10);
			}
			son_x++;
		}
		son_y++;
	}

	tem6 = satirList;
	print_dots(tem6);
	setcolor(10);
	tem7 = satirList;
	print_lines2(tem7);
	
	//a->	double min_kare_maaliyet
	//toplam_alan_min_kare
	//sekil_sayisi_min_kare
	
	//b->min_toplam_alan_maaliyet
	//toplam_alan_min_toplam_alan
	//kare_sayisi_min_toplam_alan
	
	//d-> double optimize_maaliyet
	//toplam_alan_optimize
	//sekil_sayisi_optimize
	
	
	printf("\n\nSONUCLAR\n");
	printf("Minimum sayida kare sonuclari\n");
	printf("Alan -> %d\n",toplam_alan_min_kare);
	printf("Sekil sayisi -> %d\n",sekil_sayisi_min_kare);
	printf("Alan maaliyeti-> %lf\n",toplam_alan_min_kare * birim_sondaj_maaliyeti);
	printf("Platform maaliyet-> %lf\n",sekil_sayisi_min_kare * birim_platform_maaliyeti);
	printf("Minimum kare maaliyeti -> %lf\n",min_kare_maaliyet);
	
	double fark_min_kare = (min_kare_maaliyet - double(rezerv_toplami))*-1;
	printf("Minimum kare kari -> %lf\n",fark_min_kare);
	
	printf("\n");
	
	printf("Minimum toplam alan durumu\n");
	printf("Alan -> %d\n",toplam_alan_min_toplam_alan);
	printf("Sekil sayisi -> %d\n",kare_sayisi_min_toplam_alan);
	printf("Alan maaliyeti-> %lf\n",toplam_alan_min_toplam_alan * birim_sondaj_maaliyeti);
	printf("Platform maaliyet-> %lf\n",kare_sayisi_min_toplam_alan * birim_platform_maaliyeti);
	printf("Minimum kare maaliyeti -> %lf\n",min_toplam_alan_maaliyet);
	
	double fark_min_alan = (min_toplam_alan_maaliyet - double(rezerv_toplami))*-1;
	printf("Minimum toplam alan kari -> %lf\n",fark_min_alan);
	
	printf("\n");
	
	printf("Optimum alan-kare sayisi durumu\n");
	printf("Alan -> %d\n",toplam_alan_optimize);
	printf("Sekil sayisi -> %d\n",sekil_sayisi_optimize);
	printf("Alan maaliyeti-> %lf\n",toplam_alan_optimize * birim_sondaj_maaliyeti);
	printf("Platform maaliyet-> %lf\n",sekil_sayisi_optimize * birim_platform_maaliyeti);
	printf("Minimum kare maaliyeti -> %lf\n",optimize_maaliyet);
	
	double fark_optimum = (optimize_maaliyet - double(rezerv_toplami))*-1;
	printf("Optimum alan-kare sayisi kari -> %lf\n",fark_optimum);
	
	
	printf("\n\nEN KARLI DURUM\n");
	
	
	//esit durumuna tekrardan bak
	
	//min kare
	if(fark_min_kare > fark_min_alan && fark_min_kare > fark_optimum)
	{
		printf("En karli Minimum sayida kare durumu\n");
		printf("Alan -> %d\n",toplam_alan_min_kare);
		printf("Sekil sayisi -> %d\n",sekil_sayisi_min_kare);
		printf("Alan maaliyeti-> %lf\n",toplam_alan_min_kare * birim_sondaj_maaliyeti);
		printf("Platform maaliyet-> %lf\n",sekil_sayisi_min_kare * birim_platform_maaliyeti);
		printf("Minimum kare maaliyeti -> %lf\n",min_kare_maaliyet);
		
		printf("Minimum kare kari -> %lf\n",fark_min_kare);
	}
	
	
	//min alan
	if(fark_min_alan > fark_min_kare && fark_min_alan > fark_optimum)
	{
		printf("En karli Minimum toplam alan durumu\n");
		printf("Alan -> %d\n",toplam_alan_min_toplam_alan);
		printf("Sekil sayisi -> %d\n",kare_sayisi_min_toplam_alan);
		printf("Alan maaliyeti-> %lf\n",toplam_alan_min_toplam_alan * birim_sondaj_maaliyeti);
		printf("Platform maaliyet-> %lf\n",kare_sayisi_min_toplam_alan * birim_platform_maaliyeti);
		printf("Minimum kare maaliyeti -> %lf\n",min_toplam_alan_maaliyet);
		
		printf("Minimum toplam alan kari -> %lf\n",fark_min_alan);
	}
	
	
	//optimum
	if(fark_optimum > fark_min_kare && fark_optimum > fark_min_alan)
	{
		printf("En karli Optimum alan-kare sayisi durumu\n");
		printf("Alan -> %d\n",toplam_alan_optimize);
		printf("Sekil sayisi -> %d\n",sekil_sayisi_optimize);
		printf("Alan maaliyeti-> %lf\n",toplam_alan_optimize * birim_sondaj_maaliyeti);
		printf("Platform maaliyet-> %lf\n",sekil_sayisi_optimize * birim_platform_maaliyeti);
		printf("Minimum kare maaliyeti -> %lf\n",optimize_maaliyet);
		
		printf("Optimum alan-kare sayisi kari -> %lf\n",fark_optimum);
	}

	getch();
	closegraph();
	curl_easy_cleanup(curl);
}
