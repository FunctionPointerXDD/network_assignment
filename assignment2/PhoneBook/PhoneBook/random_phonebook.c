
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <wincrypt.h>
#include "PhoneBook.h"
#include "random_phonebook.h"
#include "ui.h"

void	GenRandomName(char *param)
{
	const char* name = NULL;
	const char* names[] = { "chansjeo", "minjung", "minsu", "chulsu", "dambi", \
							"trump", "harris", "putin", "steve", "musk", "bill", \
							"mark", "bezos", "nadella", "linus", "tomsen", "ritchie", \
							"stallman", "charlse", "rachmaninov", "chopin", "liszt", \
							"benjamin", "oliver", "theodore", "alexander", "jameson", \
							"wyatt", "nolan", "lucas", "cooper", "dylane", "charlie", \
							"jordan", "rowan", "luke", "michael", "david", "jaxon", \
							"Adia", "Aella", "Alazne", "Alora", "Annika", "Axelle", \
							"Birgitta", "Baylin", "Belleann", "Beryl", "Bronwyn", \
							"Caitronia", "Constance", "Charmaine", "Calista", "Carys", \
							"Clover", "Ceinwen", "Casiane", "Dallis", "Damaris", "Darcy", \
							"Echo", "Eilish", "Ellasandra", "Ellery", "Elowynne", "Enola", \
							"Eudora", "Fable", "Fia", "Fay", "Fern", "Gaia", "Giustina", \
							"Grier", "Haisley", "Honey", "Ilaria", "Ione", "Irie", "Isabeau", \
							"Jara", "Jessamine", "Juni", "Katana", "Kelilah", "Kerensa", "Kiko" };
							

	int index = rand() % (sizeof(names) / sizeof(names[0]));
	name = names[index];
	sprintf_s(param, NAME_SIZE, "%s", name);
}

void	GenRandomAddress(char *param)
{
	const char* city_name = NULL;
	const char* city[] = { "Seoul", "Gyeongi", "Jeonbuk", "GangWon", "ChungBuk", \
				"ChungNam", "JeonNam", "GyeongBuk", "GyeongNam", "DaeGu", "Ulsan", \
				"Busan", "Dajeon", "JaeJu" };

	int index = rand() % (sizeof(city) / sizeof(city[0]));
	city_name = city[index];
	sprintf_s(param, ADDRESS_SIZE, "%s", city_name);
}

void	GenRandomNumber(char *param)
{
	HCRYPTPROV		hProvider;
	unsigned int	random_value;
	BYTE			pbData[16] = { 0 };

	random_value = 0;
	if (!CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		fprintf(stderr, "CryptAcquireContext failed\n");
		exit(1);
	}
	if (!CryptGenRandom(hProvider, sizeof(random_value), (BYTE*)&random_value))
	{
		fprintf(stderr, "CryptGenRandom failed\n");
		CryptReleaseContext(hProvider, 0);
		exit(1);
	}
	CryptReleaseContext(hProvider, 0);

	random_value = random_value % 100000000;
	sprintf_s(param, PHONE_SIZE, "010%08u", random_value);
}

int	CreateRandomPhoneBook(Table *table)
{
	FILE* test_fp = NULL;
	size_t FileLength = 0;
	errno_t err = 0;

	err = fopen_s(&test_fp, "TEST_PhoneBook.dat", "wb+");
	if (err)
		error_msg("fopen_s failed!");

	srand(time(NULL));

	fseek(test_fp, 0, SEEK_SET);
	for (size_t i = 0; i < TOTAL_TEST_ROWS; ++i)
	{
		Row row = { 0 };

		row.id = i + 1;
		GenRandomName(&row.name);
		GenRandomAddress(&row.address);
		GenRandomNumber(&row.phone);
		if (fwrite(&row, ROW_SIZE, 1, test_fp) != 1)
			error_msg("fwrite_failed!");
		FileLength += ROW_SIZE;
	}
	printf("File length: %llu bytes\n", FileLength);
	fseek(test_fp, 0, SEEK_SET);
	table->fp_start = test_fp;
	table->file_length = FileLength;
	table->total_row_cnt = FileLength / ROW_SIZE;
	table->index_end = table->total_row_cnt;
	table->index_arr_size = table->total_row_cnt;
	table->index_arr = GetIndexArray(table);
	return 0;
}

