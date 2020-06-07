//eth32nix Shaders 
// Credits: *nixCoders Team 2008

solidAChams
{
	tessSize 100
        deformVertexes wave 100 square 0 0 3 0 
        deformVertexes normal .5 .1
	{
		map *white
		blendfunc GL_SRC_ALPHA GL_ZERO
		rgbGen entity
		alphaGen const 1.0
	}
}

statbar
{
	nomipmaps
	nopicmip
	{
		map gui/common/statbar.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

statbar2
{
	nomipmaps
	nopicmip
	{
		map gui/common/statbar2.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

combtn_left
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_button_left.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

combtn_center
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_button_cen.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

combtn_right
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_button_right.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

comsel_left
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_select_left.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

comsel_center
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_select_cen.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

comsel_right
{
	nomipmaps
	nopicmip
	{
		map gui/common/com_select_right.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}
