// =============================================================================
// -----------------------------------------------------------------------------
// GetArt
// -----------------------------------------------------------------------------

#include <stdio.h>

// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
int GetPointer (FILE *a0)
{
	int d0, d1, d2, d3;
	d3 = fgetc (a0);
	d2 = fgetc (a0);
	d1 = fgetc (a0);
	d0 = fgetc (a0);
	d0 <<= 0x08;
	d0 += d1;
	d0 <<= 0x08;
	d0 += d2;
	d0 <<= 0x08;
	d0 += d3;
	return (d0);
}
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
void SavePointer (int d0, FILE *a1)
{
	fputc (d0, a1);
	d0 >>= 0x08;
	fputc (d0, a1);
	d0 >>= 0x08;
	fputc (d0, a1);
	d0 >>= 0x08;
	fputc (d0, a1);
}
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
int GetColourMatch (FILE *a0, int PalRam[])
{
	int d0, d1, d2, d3, d4, PalPoint;
	d0 = fgetc (a0);
	d1 = fgetc (a0);
	d2 = fgetc (a0);
	d4 = 0x00;
	PalPoint = 0x00;
	while (PalPoint != 0x30&&d4 != 0x03)
	{
		d4 = 0x00;
		d3 = PalRam[PalPoint];
		PalPoint += 0x01;
		if (d3 == d0)
		{
			d4 += 0x01;
		}
		d3 = PalRam[PalPoint];
		PalPoint += 0x01;
		if (d3 == d1)
		{
			d4 += 0x01;
		}
		d3 = PalRam[PalPoint];
		PalPoint += 0x01;
		if (d3 == d2)
		{
			d4 += 0x01;
		}
	}
	PalPoint /= 0x03;
	d0 = PalPoint;
	d0 -= 0x01;
	if (d4 != 0x03)
	{
		d0 = 0xFF;
	}
	return (d0);
}
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
int GetArtByte (int ArtRam[], int ArtPoint)
{
	int d0, d1, d2;
	d0 = ArtRam[ArtPoint];
	d1 = d0;
	d2 = d1;
	d2 <<= 0x04;
	d1 >>= 0x04;
	d1 &= 0x0F;
	d1 += d2;
	d0 <<= 0x08;
	d0 &= 0xFF00;
	d1 &= 0xFF;
	d0 += d1;
	return (d0);
}
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
int CreateFlips (int ArtRam[], int ArtPoint, int TileSpace[])
{
	int d0, d1;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	b4 = 0x00;
	b5 = 0x00;
	b6 = 0x00;
	b7 = 0x00;
	b2 = 0x08;
	while (b2 != 0x00)
	{
		b3 = 0x04;
		while (b3 != 0x00)
		{
			d0 = GetArtByte (ArtRam, ArtPoint);
			ArtPoint += 0x01;
			d1 = d0;
			d0 >>= 0x08;
			TileSpace[b4+0x00] = d0;
			TileSpace[b5+0x23] = d1;
			TileSpace[b6+0x5C] = d0;
			TileSpace[b7+0x7F] = d1;
			b4 += 0x01;
			b5 -= 0x01;
			b6 += 0x01;
			b7 -= 0x01;
			b3 -= 0x01;
		}
		b4 += 0x00;
		b5 += 0x08;
		b6 -= 0x08;
		b7 -= 0x00;
		b2 -= 0x01;
	}
	return (ArtPoint);
}
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
void WriteByteDigit (int d0, FILE *a1)
{
	int d1;
	d1 = d0;
	d0 >>= 0x04;
	d0 &= 0x0F;
	d1 &= 0x0F;
	if (d0 >= 0x0A)
	{
		d0 += 0x07;
	}
	d0 += 0x30;
	fputc (d0, a1);
	if (d1 >= 0x0A)
	{
		d1 += 0x07;
	}
	d1 += 0x30;
	fputc (d1, a1);
}
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------

int main (void)
{
	printf ("GetArt - By MarkeyJester\n");

	FILE *a0, *a1;
	int EX, d0, d1, d2, d3, d4, d5, d6, d7;
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
	printf ("\n Status: Palette Data\n");
// -----------------------------------------------------------------------------
	printf (" >> Opening Palette File\n");
	if ((a0 = fopen ("Palette.bmp", "rb")) == NULL)
	{
		printf (" >> Error: Could not open 'Palette.bmp'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
// -----------------------------------------------------------------------------
	printf (" >> Loading & Dumping Palette Data\n");
	if ((a1 = fopen ("Palette.bin", "wb")) == NULL)
	{
		fclose (a0);
		printf (" >> Error: Could not open 'Palette.bit'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
	fseek (a0, 0x36, SEEK_SET);
	int PalRam[0x30], PalPoint = 0x00;
	d3 = 0x10;
	while (d3 != 0x00)
	{
		d0 = fgetc (a0);
		PalRam[PalPoint] = d0;
		PalPoint += 0x01;
		d0 >>= 0x04;
		d0 &= 0x0E;
		d1 = fgetc (a0);
		PalRam[PalPoint] = d1;
		PalPoint += 0x01;
		d1 &= 0xE0;
		d2 = fgetc (a0);
		PalRam[PalPoint] = d2;
		PalPoint += 0x01;
		d2 >>= 0x04;
		d2 &= 0x0E;
		d1 += d2;
		fputc (d0, a1);
		fputc (d1, a1);
		fseek (a0, 0x15, SEEK_CUR);
		d3 -= 0x01;
	}
// -----------------------------------------------------------------------------
	fclose (a0);
	fclose (a1);
	printf (" >> Complete\n");
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
	printf ("\n Status: Art Data\n");
// -----------------------------------------------------------------------------
	printf (" >> Opening Bitmap File\n");
	if ((a0 = fopen ("Bitmap.bmp", "rb")) == NULL)
	{
		printf (" >> Error: Could not open 'Bitmap.bmp'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
// -----------------------------------------------------------------------------
	printf (" >> Obtaining Bitmap Information\n");
	fseek (a0, 0x12, SEEK_SET);
	int XSIZE = GetPointer (a0);
	int YSIZE = GetPointer (a0);
	printf ("   ~Info:\n     X Size of bitmap: $%0.8X\n     Y Size of bitmap: $%0.8X\n", XSIZE, YSIZE);
// -----------------------------------------------------------------------------
	printf (" >> Padding Bitmap Art\n");
	if ((a1 = fopen ("Pad.bmp", "wb")) == NULL)
	{
		fclose (a0);
		printf (" >> Error: Could not open 'Pad.bmp'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
	fseek (a0, 0x00, SEEK_SET);
	d1 = 0x36;
	while (d1 != 0x00)
	{
		d0 = fgetc (a0);
		fputc (d0, a1);
		d1 -= 0x01;
	}
	fseek (a1, 0x12, SEEK_SET);
	d1 = XSIZE;
	int XORI = XSIZE;
	d1 &= 0x07;
	if (d1 != 0x00)
	{
		XSIZE += 0x08;
	}
	XSIZE &= 0xFFFFFFF8;
	int XPAD = XSIZE;
	XPAD -= XORI;
	SavePointer (XSIZE, a1);

	d1 = YSIZE;
	int YORI = d1;
	d1 &= 0x07;
	if (d1 != 0x00)
	{
		YSIZE += 0x08;
	}
	YSIZE &= 0xFFFFFFF8;
	int YPAD = YSIZE;
	YPAD -= YORI;
	SavePointer (YSIZE, a1);
	printf ("   ~Info:\n     X Pixels Padded:  $%0.8X\n     Y Pixels Padded:  $%0.8X\n", XPAD, YPAD);
	fseek (a0, 0x36, SEEK_SET);
	fseek (a1, 0x36, SEEK_SET);
	while (YPAD != 0x00)
	{
		d0 = XSIZE;
		while (d0 != 0x00)
		{
			fputc (PalRam[0x00], a1);
			fputc (PalRam[0x01], a1);
			fputc (PalRam[0x02], a1);
			d0 -= 0x01;
		}
		YPAD -= 0x01;
	}
	d2 = YORI;
	while (d2 != 0x00)
	{
		d1 = XORI;
		while (d1 != 0x00)
		{
			d0 = fgetc (a0);
			fputc (d0, a1);
			d0 = fgetc (a0);
			fputc (d0, a1);
			d0 = fgetc (a0);
			fputc (d0, a1);
			d1 -= 0x01;
		}
	// Padding...
		d0 = ftell (a0);
		d0 -= 0x36;
		d0 &= 0x03;
		if (d0 != 0x00)
		{
			d1 = 0x04;
			d1 -= d0;
			d0 = d1;
		}
		fseek (a0, d0, SEEK_CUR);
	// ...ughh
		d1 = XPAD;
		while (d1 != 0x00)
		{
			fputc (PalRam[0x00], a1);
			fputc (PalRam[0x01], a1);
			fputc (PalRam[0x02], a1);
			d1 -= 0x01;
		}
		d2 -= 0x01;
	}
	fclose (a0);
	fclose (a1);
// -----------------------------------------------------------------------------
	if ((a0 = fopen ("Pad.bmp", "rb")) == NULL)
	{
		fclose (a0);
		printf (" >> Error: Could not open 'Pad.bmp'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
// -----------------------------------------------------------------------------
	printf (" >> Converting Art Data\n");
	int ArtRam[0x00020000], ArtPoint = 0x00;
	d0 = XSIZE;
	d0 *= 0x03;
	d1 = YSIZE;
	d1 *= d0;
	d1 -= d0;
	d1 += 0x36;
	fseek (a0, d1, SEEK_SET);
	d5 = YSIZE;
	d5 >>= 0x03;
	while (d5 != 0x00)
	{
		d4 = XSIZE;
		d4 >>= 0x03;
		while (d4 != 0x00)
		{
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			d3 = 0x08;
			while (d3 != 0x00)
			{
				d2 = 0x04;
				while (d2 != 0x00)
				{
					if ((d0 = GetColourMatch (a0, PalRam)) == 0xFF)
					{
						printf (" >> Error: One of the pixel colours do not match the palette\n");
						d0 = ftell (a0);
						d0 -= 0x03;
						printf ("   ~Info:\n     Offset: '$%0.8X'\n", d0);
						printf ("\nPress enter key to exit...\n");
						EX = getc (stdin);
						return 0;
					}
					if ((d1 = GetColourMatch (a0, PalRam)) == 0xFF)
					{
						printf (" >> Error: One of the pixel colours do not match the palette\n");
						d0 = ftell (a0);
						d0 -= 0x03;
						printf ("   ~Info:\n     Offset: '$%0.8X'\n", d0);
						printf ("\nPress enter key to exit...\n");
						EX = getc (stdin);
						return 0;
					}
					d0 <<= 0x04;
					d0 += d1;
					ArtRam[ArtPoint] = d0;
					ArtPoint += 0x01;
					d2 -= 0x01;
				}
				d0 = XSIZE;
				d0 *= 0x03;
				d0 += 0x18;
				d0 = -d0;
				d3 -= 0x01;
				if (d3 != 0x00)
				{
					fseek (a0, d0, SEEK_CUR);
				}
				d7 = d0;
			}
// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			d0 = XSIZE;
			d0 *= 0x03;
			d0 *= 0x08;
			d0 += 0x18;
			d0 += d7;
			d4 -= 0x01;
			if (d4 != 0x00)
			{
				fseek (a0, d0, SEEK_CUR);
			}
			d7 = d0;
		}
		d0 = XSIZE;
		d0 *= 0x03;
		d1 = d0;
		d0 *= 0x08;
		d0 += d1;
		d0 = -d0;
		d0 += d7;
		fseek (a0, d0, SEEK_CUR);
		d5 -= 0x01;
	}
// -----------------------------------------------------------------------------
	fclose (a0);
	printf (" >> Complete\n");
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
	printf ("\n Status: Optimising & Generating Mappings\n");
// -----------------------------------------------------------------------------
	printf (" >> Creating Map File\n");
	if ((a1 = fopen ("Map.bin", "wb")) == NULL)
	{
		printf (" >> Error: Could not open 'Map.bin'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
// -----------------------------------------------------------------------------
	printf (" >> Optimising Tiles & Writing Mappings\n");
	int MaxPoint = ArtPoint;
	int TileSpace[0x80], TilePoint;
	ArtPoint = 0x00;
	int StackArt[0x00020000], StackPoint = 0x00, StackMain = 0x00;
	int MapCount = 0x0000, MapSave = 0x0000;
	while (ArtPoint != MaxPoint)
	{
		ArtPoint = CreateFlips (ArtRam, ArtPoint, TileSpace);
		StackPoint = 0x00;
		d6 = 0x00;
		while (StackPoint != StackMain)
		{
			d2 = 0x00;
			d6 = 0x00;
			d5 = 0x04;
			while (d5 != 0x00)
			{
				d4 = 0x00;
				d3 = 0x20;
				while (d3 != 0x00)
				{
					d0 = StackArt[StackPoint];
					StackPoint += 0x01;
					d1 = TileSpace[d2];
					d0 &= 0xFF;
					d1 &= 0xFF;
					d2 += 0x01;
					if (d1 != d0)
					{
						d4 = 0x01;
					}
					d3 -= 0x01;
				}
				StackPoint -= 0x20;
				if (d4 == 0x00)
				{
					MapSave = StackPoint;
					MapSave /= 0x20;
					int MPID = 0x04 - d5;
					MPID <<= 0x0B;
					MPID &= 0x1800;
					MapSave &= 0x07FF;
					MapSave += MPID;
					StackPoint = StackMain;
					StackPoint -= 0x20;
					d6 = 0x01;
					d5 = 0x01;
				}
				d5 -= 0x01;
			}
			StackPoint += 0x20;
		}
		if (d6 == 0x00)
		{
			MapSave = MapCount;
			MapCount += 0x01;
			ArtPoint -= 0x20;
			d0 = 0x20;
			while (d0 != 0x00)
			{
				StackArt[StackPoint] = ArtRam[ArtPoint];
				ArtPoint += 0x01;
				StackPoint += 0x01;
				d0 -= 0x01;
			}
			StackMain = StackPoint;
		}
		d0 = MapSave;
		d0 >>= 0x08;
		fputc (d0, a1);
		fputc (MapSave, a1);
	}
	fclose (a1);
// -----------------------------------------------------------------------------
	printf (" >> Saving Optimised Art\n");
	if ((a1 = fopen ("Art.bin", "wb")) == NULL)
	{
		printf (" >> Error: Could not open 'Art.bin'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
	d0 = 0x00;
	while (d0 != StackMain)
	{
		fputc (StackArt[d0], a1);
		d0 += 0x01;
	}
// -----------------------------------------------------------------------------
	fclose (a1);
	printf (" >> Complete\n");
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
	printf ("\n Status: Finalising Data\n");
// -----------------------------------------------------------------------------
	printf (" >> Opening Document\n");
	if ((a1 = fopen ("Info.txt", "wb")) == NULL)
	{
		printf (" >> Error: Could not open 'Info.txt'\n");
		printf ("\nPress enter key to exit...\n");
		EX = getc (stdin);
		return 0;
	}
// -----------------------------------------------------------------------------
	printf (" >> Writing Statistics\n");
	fputc (0x0D, a1);
	fputc (0x0A, a1);
	fputs (" Statistics", a1);
	fputc (0x0D, a1);
	fputc (0x0A, a1);
	fputc (0x0D, a1);
	fputc (0x0A, a1);
	fputs ("   Number of horizontal tiles (X):           $", a1);
	d0 = XSIZE /= 0x08;
	WriteByteDigit (d0, a1);
	fputc (0x0D, a1);
	fputc (0x0A, a1);
	fputs ("   Number of vertical tiles (Y):             $", a1);
	d0 = YSIZE /= 0x08;
	WriteByteDigit (d0, a1);
	fputc (0x0D, a1);
	fputc (0x0A, a1);
// -----------------------------------------------------------------------------
	fclose (a1);
	printf (" >> Complete\n");
// -----------------------------------------------------------------------------
// =============================================================================
// -----------------------------------------------------------------------------
	printf ("\n Status: Complete\n");
	printf ("\nPress enter key to exit...\n");
	EX = getc (stdin);
	return 0;
}
// -----------------------------------------------------------------------------
// =============================================================================
