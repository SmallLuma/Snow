#include "Snow/Activity.h"
#include "Snow/Timer.h"
#include "Snow/Sound.h"
#include "Snow/Globals.h"
#include "Snow/Control.h"
#include "Snow/Debug.h"
#include <stack>
#include <cstdlib>

using namespace Snow;
using namespace std;



namespace Snow{

extern Activity* nowFocus;
extern Activity* nextFocus;
extern stack<Activity*> actStack;
extern bool jumpDraw;

void JumpDraw(){
    jumpDraw = true;
}
void Goto(Activity* a)
{
    if(a != nowFocus)
        nextFocus = a;
}

void Call(Activity* a){
    actStack.push(nowFocus);
    nowFocus = a;
    a -> OnShow();
}
//Activity* GetParent()
//{return actStack.top();}

void Return(){
    nowFocus -> OnHide();
    nowFocus = actStack.top();
    actStack.pop();
}

//void ActivityDrawProc() //活动刷新一次处理
//{
    /*  取消活动间跳转动画功能   *
    if(isGotoing){  //如果正在跳转
        bool lastFinished = false;
        bool nowFinished = false;

        if(lastFocus != nullptr && !lastFinished) {
            if(lastFocus -> GetAnimationOnHide() != nullptr){
                lastFocus -> GetAnimationOnHide() -> OnNext();   //绘制上个活动
                if(lastFocus -> GetAnimationOnHide() -> Finished()) lastFinished = true;
            }else lastFinished = true;
            lastFocus -> OnDraw();
        }else lastFinished = true;

        if(nowFocus != nullptr && !nowFinished) {
            if(nowFocus -> GetAnimationOnShow() != nullptr){
                nowFocus -> GetAnimationOnShow() -> OnNext();   //绘制下个活动
                if(nowFocus -> GetAnimationOnShow() -> Finished()) nowFinished = true;
            }else nowFinished = true;
            nowFocus -> OnDraw();
        }else nowFinished = true;

        if(lastFinished && nowFinished) isGotoing = false;
    }

    //两个动画结束后关闭跳转状态
    else{
        */
        //nowFocus -> OnNext();
        //nowFocus -> OnDraw();
    //}
//}

void Exit(int exitcode)
{
    if(exitcode < 0) exit(exitcode);
    else {
        if(nowFocus != nullptr) nowFocus -> OnHide();
        while(!actStack.empty()){
            Activity*p = actStack.top();
            p -> OnHide();
            actStack.pop();
        }
        SDL_Event e;
        e.type = SDL_QUIT;
        if(exitcode == 0) SDL_PushEvent(&e);
        else exit(exitcode);
    }
}

void Run(Activity* start)
{
    SDL_SetRenderDrawBlendMode(pRnd,SDL_BLENDMODE_BLEND);

    nowFocus = nullptr;
    nextFocus = start;
    //nowFocus -> OnShow();
    /* 主循环部分 */
    SDL_Event e;
    Timer FPSKiller;

    while(1){

        /**** 如果有Goto消息则执行Goto ****/
        if(nextFocus != nullptr){
                if(nowFocus != nullptr) nowFocus -> OnHide();
                while(!actStack.empty()){
                    Activity*p = actStack.top();
                    p -> OnHide();
                    actStack.pop();
                }
                nowFocus = nextFocus;
                nextFocus = nullptr;
                SDL_RenderSetLogicalSize(pRnd,nowFocus -> m_logic_w,nowFocus -> m_logic_h);
                if(!(nowFocus -> m_Inited)){
                    nowFocus ->OnInit();
                    nowFocus ->m_Inited = true;
                }
                nowFocus -> OnShow();
        }

        /**** 处理当前帧所有的事件 ****/
        while(SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT) {   //如果是退出
                nowFocus -> OnHide();
                while(!actStack.empty()){
                    Activity*p = actStack.top();
                    p -> OnHide();
                    actStack.pop();
                }
                return;
            }
            else {
                if(!(nowFocus -> m_ansList.empty())){
                    bool SendToActivity = true;
                    FOR_EACH(p,nowFocus -> m_ansList.begin(),nowFocus -> m_ansList.end())  //遍历控件表
                    {
                        if((*p) -> OnEvent(e,*nowFocus)){  //发现有控件接受该信息后返回
                            SendToActivity = false;
                            break;
                        }
                    }
                    if(SendToActivity) nowFocus -> OnEvent(e);    //无控件接受消息，发送消息给活动的OnEvent()
                }else nowFocus -> OnEvent(e);
            }
        }

        /**** 绘制 ****/
        SDL_SetRenderDrawColor(pRnd,0x00,0x00,0x00,0xFF);
        nowFocus -> OnNext();   //可以取得上一帧数据
        if(jumpDraw) {
            pRnd.m_fps++;
            jumpDraw = false;
            continue;
        }
        SDL_RenderClear(pRnd);
        nowFocus -> OnDraw();

        //高精度FPS控制
        static int fps_count=0,count0t;
        static int f[FPS];
        static double ave;

        {
            int term,i,gnt;
            static int t=0;
            if(fps_count==0){
                if(t==0)
                    term=0;
                else
                    term=count0t+1000-SDL_GetTicks();
            }
            else
                term = (int)(count0t+fps_count*(1000.0/FPS))-SDL_GetTicks();

            if(term>0)
                SDL_Delay(term);

            gnt=SDL_GetTicks();

            if(fps_count==0)
                count0t=gnt;
            f[fps_count]=gnt-t;
            t=gnt;
            if(fps_count==FPS-1){
                ave=0;
                for(i=0;i<FPS;i++)
                    ave+=f[i];
                ave/=FPS;
            }
            ++fps_count;
            fps_count = fps_count%FPS ;
        }

        #ifdef _DEBUG
        char buf [8];
        sprintf(buf,"FPS:%f",1000.0/ave);
        SDL_SetWindowTitle(pRnd,buf);
        #endif // _DEBUG
        SDL_RenderPresent(pRnd);

        //FPSKiller.WaitTimer(1000/FPS);   //FPS限制
        //FPSKiller.Reset();
        pRnd.m_fps++;
    }
}

void Init(){
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDLNet_Init();

    Snow::Sound::Init();

    Snow::nowFocus = nullptr;
    Snow::nextFocus = nullptr;
}

}


