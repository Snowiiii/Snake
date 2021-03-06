#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sstream>

cv::Mat img;
int len = 60 * 4, dir = 2, posx = 400, posy = 300, *arrx, *arry, scoreold = 0,
	fx, fy, fc = 1, leninc = 20, score = 0,  slim = 0, level = 4, count = 0,
	showbonus = 0, bx, by, bc = 0, levelstat = 0;
std::string sha_score;
std::ostringstream sha_temp;

class SHA256
{
protected:
	typedef unsigned char uauto8;
	typedef unsigned int uauto32;
	typedef unsigned long long uauto64;

	const static uauto32 sha256_k[];
	static const unsigned int SHA224_256_BLOCK_SIZE = (512 / 8);

public:
	void init();
	void update(const unsigned char *message, unsigned int len);
	void final(unsigned char *digest);
	static const unsigned int DIGEST_SIZE = (256 / 8);

protected:
	void transform(const unsigned char *message, unsigned int block_nb);
	unsigned int m_tot_len;
	unsigned int m_len;
	unsigned char m_block[2 * SHA224_256_BLOCK_SIZE];
	uauto32 m_h[8];
	
};

#define SHA2_SHFR(x, n) (x >> n)
#define SHA2_ROTR(x, n) ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n) ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z) ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA256_F1(x) (SHA2_ROTR(x, 2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x, 6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x, 7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x, 3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))
#define SHA2_UNPACK32(x, str)              \
	{                                      \
		*((str) + 3) = (uauto8)((x));       \
		*((str) + 2) = (uauto8)((x) >> 8);  \
		*((str) + 1) = (uauto8)((x) >> 16); \
		*((str) + 0) = (uauto8)((x) >> 24); \
	}
#define SHA2_PACK32(str, x)                                                                                                                \
	{                                                                                                                                      \
		*(x) = ((uauto32) * ((str) + 3)) | ((uauto32) * ((str) + 2) << 8) | ((uauto32) * ((str) + 1) << 16) | ((uauto32) * ((str) + 0) << 24); \
	}

const unsigned int SHA256::sha256_k[64] = 
	{0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void SHA256::transform(const unsigned char *message, unsigned int block_nb)
{
	uauto32 w[64];
	uauto32 wv[8];
	uauto32 t1, t2;
	const unsigned char *sub_block;
	int i,j;
	for (i = 0; i < (int)block_nb; i++)
	{
		sub_block = message + (i << 6);
		for (j = 0; j < 16; j++)
		{
			SHA2_PACK32(&sub_block[j << 2], &w[j]);
		}
		for (j = 16; j < 64; j++)
		{
			w[j] = SHA256_F4(w[j - 2]) + w[j - 7] + SHA256_F3(w[j - 15]) + w[j - 16];
		}
		for (j = 0; j < 8; j++)
		{
			wv[j] = m_h[j];
		}
		for (j = 0; j < 64; j++)
		{
			t1 = wv[7] + SHA256_F2(wv[4]) + SHA2_CH(wv[4], wv[5], wv[6]) + sha256_k[j] + w[j];
			t2 = SHA256_F1(wv[0]) + SHA2_MAJ(wv[0], wv[1], wv[2]);
			wv[7] = wv[6];
			wv[6] = wv[5];
			wv[5] = wv[4];
			wv[4] = wv[3] + t1;
			wv[3] = wv[2];
			wv[2] = wv[1];
			wv[1] = wv[0];
			wv[0] = t1 + t2;
		}
		for (j = 0; j < 8; j++)
		{
			m_h[j] += wv[j];
		}
	}
}

void SHA256::init()
{
	m_h[0] = 0x6a09e667;
	m_h[1] = 0xbb67ae85;
	m_h[2] = 0x3c6ef372;
	m_h[3] = 0xa54ff53a;
	m_h[4] = 0x510e527f;
	m_h[5] = 0x9b05688c;
	m_h[6] = 0x1f83d9ab;
	m_h[7] = 0x5be0cd19;
	m_len = 0;
	m_tot_len = 0;
}

void SHA256::update(const unsigned char *message, unsigned int len)
{
	unsigned int block_nb;
	unsigned int new_len, rem_len, tmp_len;
	const unsigned char *shifted_message;
	tmp_len = SHA224_256_BLOCK_SIZE - m_len;
	rem_len = len < tmp_len ? len : tmp_len;
	memcpy(&m_block[m_len], message, rem_len);
	if (m_len + len < SHA224_256_BLOCK_SIZE)
	{
		m_len += len;
		return;
	}
	new_len = len - rem_len;
	block_nb = new_len / SHA224_256_BLOCK_SIZE;
	shifted_message = message + rem_len;
	transform(m_block, 1);
	transform(shifted_message, block_nb);
	rem_len = new_len % SHA224_256_BLOCK_SIZE;
	memcpy(m_block, &shifted_message[block_nb << 6], rem_len);
	
	m_len = rem_len;
	m_tot_len += (block_nb + 1) << 6;
}

void SHA256::final(unsigned char *digest)
{
	unsigned int block_nb;
	unsigned int pm_len;
	unsigned int len_b;
	int i;
	block_nb = (1 + ((SHA224_256_BLOCK_SIZE - 9) < (m_len % SHA224_256_BLOCK_SIZE)));
	len_b = (m_tot_len + m_len) << 3;
	pm_len = block_nb << 6;
	memset(m_block + m_len, 0, pm_len - m_len);
	m_block[m_len] = 0x80;
	SHA2_UNPACK32(len_b, m_block + pm_len - 4);
	transform(m_block, block_nb);
	for (i = 0; i < 8; i++)
	{
		SHA2_UNPACK32(m_h[i], &digest[i << 2]);
	}
}

std::string sha256(std::string input)
{
	unsigned char digest[SHA256::DIGEST_SIZE];
	memset(digest, 0, SHA256::DIGEST_SIZE);

	SHA256 ctx = SHA256();
	ctx.init();
	ctx.update((unsigned char *)input.c_str(), input.length());
	ctx.final(digest);

	char buf[2 * SHA256::DIGEST_SIZE + 1];
	buf[2 * SHA256::DIGEST_SIZE] = 0;
	for (unsigned int i = 0; i < SHA256::DIGEST_SIZE; i++)
		sprintf(buf + i * 2, "%02x", digest[i]);
	return std::string(buf);
}

void exitnow()
{
	std::cout << "Game Over!\nScore: " << score << std::endl;

	if (score > scoreold)
	{
		std::cout << "You have got a new high score!" << std::endl;
		std::ofstream fout;
		fout.open("snake.txt", std::ios::out);
		sha_temp.str("");
		sha_temp << score;
		fout << sha256(sha_temp.str());
		fout.close();
	}
	std::cout << "Gramercy..." << std::endl;
	exit(0);
}

void setDir(char a)
{
	if (a == 'a' && dir != 2)
		dir = 1;
	if (a == 'd' && dir != 1)
		dir = 2; 
	if (a == 'w' && dir != 4)
		dir = 3;
	if (a == 's' && dir != 3)
		dir = 4;
}

void setPos()
{

	if (dir == 1)
	{
		for (auto i = len - 1; i > 0; i--)
			arrx[i] = arrx[i - 1];
		for (auto i = len - 1; i > 0; i--)
			arry[i] = arry[i - 1];
		arrx[0] -= 1;
		if (arrx[0] < 0)
			arrx[0] = 799;
	}
	if (dir == 2)
	{
		for (auto i = len - 1; i > 0; i--)
			arrx[i] = arrx[i - 1];
		for (auto i = len - 1; i > 0; i--)
			arry[i] = arry[i - 1];
		arrx[0] += 1;
		if (arrx[0] > 799)
			arrx[0] = 0;
	}
	if (dir == 3)
	{
		for (auto i = len - 1; i > 0; i--)
			arry[i] = arry[i - 1];
		for (auto i = len - 1; i > 0; i--)
			arrx[i] = arrx[i - 1];
		arry[0] -= 1;
		if (arry[0] < 0)
			arry[0] = 599;
	}
	if (dir == 4)
	{
		for (auto i = len - 1; i > 0; i--)
			arry[i] = arry[i - 1];
		for (auto i = len - 1; i > 0; i--)
			arrx[i] = arrx[i - 1];
		arry[0] += 1;
		if (arry[0] > 599)
			arry[0] = 0;
	}
}

void giveFood()
{
	if (fc)
	{
		srand(::count);
		fx = 50 + rand() % 700;
		fy = 50 + rand() % 500;
		fc = 0;
	}
	for (auto i = fx - (12 - level); i <= fx + (12 - level); i++)
		for (auto j = fy - (12 - level); j <= fy + (12 - level); j++)
			img.at<uchar>(j, i) = 200;
	::count++;
}

void sprauto()
{
	for (auto i = 0; i < img.rows; i++)
		for (auto j = 0; j < img.cols; j++)
			img.at<uchar>(i, j) = 0;
	for (auto i = 0; i < len; i++)
	{
		auto x = arrx[i], y = arry[i];
		img.at<uchar>(y, x) = 255;
	}
}

void incLen()
{
	auto xd = arrx[len - 2] - arrx[len - 1];
	auto yd = arry[len - 2] - arry[len - 1];
	arrx = (int *)realloc(arrx, (len + leninc) * sizeof(int));
	arry = (int *)realloc(arry, (len + leninc) * sizeof(int));
	if (xd > 0)
	{
		for (auto i = len; i < len + leninc; i++)
		{
			arrx[i] = arrx[i - 1] - 1;
			if (arrx[i] < 0)
				arrx[i] += 800;
		}
		for (auto i = len; i < len + leninc; i++)
			arry[i] = arry[i - 1];
	}
	if (xd < 0)
	{
		for (auto i = len; i < len + leninc; i++)
		{
			arrx[i] = arrx[i - 1] + 1;
			if (arrx[i] > 799)
				arrx[i] -= 800;
		}
		for (auto i = len; i < len + leninc; i++)
			arry[i] = arry[i - 1];
	}
	if (yd > 0)
	{
		for (auto i = len; i < len + leninc; i++)
		{
			arry[i] = arry[i - 1] - 1;
			if (arry[i] < 0)
				arry[i] += 800;
		}
		for (auto i = len; i < len + leninc; i++)
			arrx[i] = arrx[i - 1];
	}
	if (yd < 0)
	{
		for (auto i = len; i < len + leninc; i++)
		{
			arry[i] = arry[i - 1] + 1;
			if (arry[i] > 799)
				arry[i] -= 800;
		}
		for (auto i = len; i < len + leninc; i++)
			arrx[i] = arrx[i - 1];
	}
	len += leninc;
	slim = 0;
}

void checkFood()
{
	int flag = 0, k;
	for (auto i = fx - (12 - level); i <= fx + (12 - level); i++)
		for (auto j = fy - (12 - level); j <= fy + (12 - level); j++)
			for (k = 0; k < len; k++)
				if (arrx[k] == i && arry[k] == j)
				{
					flag = 1;
					break;
				}
	if (flag == 1)
	{
		incLen();
		score++;
		if (!score % 4)
		{
			showbonus = 1;
			bc = 1;
		}
		fc = 1;
	}
}

void gameOver()
{
	auto lennow = len;
	if (slim > 10)
		lennow -= 5;
	auto x = arrx[0], y = arry[0];
	for (auto i = 1; i < lennow; i++)
		if (arrx[i] == x && arry[i] == y)
			exitnow();
}

void prautoBonus()
{
	if (bc == 1)
	{
		bx = 50 + rand() % 700;
		by = 50 + rand() % 500;
	}
	if (!(bc / 10) % 2)
		for (auto i = bx - 6; i <= bx + 6; i++)
			for (auto j = by - 6; j <= by + 6; j++)
				img.at<uchar>(j, i) = 200;
	bc++;
}

void checkBonus()
{
	int flag = 0, k;
	for (auto i = bx - 6; i <= bx + 6; i++)
		for (auto j = by - 6; j <= by + 6; j++)
			for (k = 0; k < len; k++)
				if (arrx[k] == i && arry[k] == j)
				{
					flag = 1;
					break;
				}
	if (flag)
	{
		incLen();
		incLen();
		score += 2;
		if (!score % 4)
		{
			showbonus = 1;
			bc = 1;
		}
		showbonus = 0;
		bc = 0;
	}
	if (bc > 80)
	{
		showbonus = 0;
		bc = 0;
	}
}

void itoa(int num, char ch[])
{
	if (!num)
	{
		ch[0] = '0';
		ch[1] = '\0';
		return;
	}
	auto c = 0;
	while (num)
	{
		ch[c++] = (num % 10) + '0';
		num /= 10;
	}
	ch[c] = '\0';
	for (auto i = 0; i < c / 2; i++)
	{
		char temp = ch[i];
		ch[i] = ch[c - i - 1];
		ch[c - i - 1] = temp;
	}
}

int main()
{
	std::ifstream fin;
	fin.open("snake.txt", std::ios::in);
	fin >> sha_score;
	fin.close();


	auto i = 0;
	while (true and !sha_score.empty())
	{
		sha_temp.str("");
		sha_temp << i;
		if (sha_score == sha256(sha_temp.str()))
			break; 
		i++;
	}
	scoreold = i;
	arrx = (int *)malloc(len * sizeof(int));
	arry = (int *)malloc(len * sizeof(int));
	for (auto i = 0; i < len; i++)
	{
		arrx[i] = posx - i;
		arry[i] = posy;
	}
	img = cv::Mat::zeros(600, 800, CV_8U);
	cv::namedWindow("Snake", 1);
	while (true)
	{
		if (score % 10 == 1 || score % 10 == 2)
			levelstat = 0;
		slim++;
		char ch = 'd';
		ch = cv::waitKey((101 - level * 10));
		if (ch == 'p')
			score++;
		if (ch == '\n' || ch == '\r')
			exitnow();
		if (ch == ' ')
			fc = 1;
		setDir(ch);
		for (auto i = 1; i <= 8 + (1.5 * level); i++)
			setPos();
		gameOver();
		sprauto();
		if (showbonus)
		{
			prautoBonus();
			checkBonus();
		}
		giveFood();
		checkFood();
		char adder[4];
		itoa(score, adder);
		char s[15] = "Score: \0";
		strcat(s, adder); 
		char sold[17] = "High score: \0";
		itoa(scoreold, adder);
		strcat(sold, adder); 
		cv::Point p;
		p.x = 0;
		p.y = 20;
		putText(img, s, p, 0, 0.5, 150);
		p.x = 630;
		putText(img, sold, p, 0, 0.5, 150);
		cv::imshow("Snakes", img);
	}
}