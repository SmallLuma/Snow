#include "Snow/Debug.h"
#include "Snow/Surface.h"

using namespace Snow;

Surface::Surface(){}

Surface::Surface(SDL_Surface* p)
{
    m_sur = p;
}


Surface::Surface(const std::string& f){
    Load(f);
}

void Surface::ConvertFormat(Uint32 sdlFormat)
{
    auto pSur = SDL_ConvertSurfaceFormat(m_sur,sdlFormat,0);
    SDL_FreeSurface(m_sur);
    m_sur = pSur;
}

void Surface::SetAlpha(Uint8 alp)
{
    SDL_SetSurfaceAlphaMod(m_sur,alp);
}

void Surface::SetBlend(SDL_BlendMode blendMode)
{
    SDL_SetSurfaceBlendMode(m_sur,blendMode);
}

void Surface::SetColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetSurfaceColorMod(m_sur,r,g,b);
}

void Surface::Lock()
{
    SDL_LockSurface(m_sur);
}

void Surface::Unlock()
{
    SDL_UnlockSurface(m_sur);
}

void Surface::Load(const std::string& f){
    PNT("SURFACE CLEAR");
    Clear();
    PNT("SURFACE FILE LOAD");
    m_file.Load(f);
    PNT("SURFACE LOAD");
    m_sur = IMG_Load_RW(m_file,m_file.Size());
    PNT("SURFACE LOADED");
    //SDL_ConvertSurfaceFormat(m_sur,SDL_PIXELFORMAT_ARGB8888,0);
    SDL_SetSurfaceRLE(m_sur,1);
    SDL_SetSurfaceBlendMode(m_sur,SDL_BLENDMODE_BLEND);
}

void Surface::Load(SDL_Surface* p){
    Clear();
    m_sur = p;
}

void Surface::Clear(){
    if(m_sur != nullptr)
		SDL_FreeSurface(m_sur);
    m_sur = nullptr;
    m_file.Free();
}

void Surface::BlitWithSubSurface(Surface& src,SDL_Rect& srcr,SDL_Rect& dst){
    SDL_BlitSurface(src,&srcr,m_sur,&dst);
}
