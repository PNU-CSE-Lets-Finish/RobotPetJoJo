/*
 * [TODO]
 * RQ-DEV-02 (로봇 펫 배터리 잔량 확인), RQ-DEV-04 (로봇 펫 방전 방지)
 * RQ-DEV-05 (로봇 펫 충돌 방지)
 * 
 * 함수 세부 구현
 */

#define RC_OK 0
#define RC_PLAYING 1

#define ACT_CODE     'a'

#define ACT_INIT     -2
#define ACT_CONNECT  -1
#define ACT_NONE      0
#define ACT_BARK      1
#define ACT_SIT       2
#define ACT_CALL      3
#define ACT_BACK      4
#define ACT_HOLD      5
#define ACT_WALK      6
#define ACT_TURN      7
#define ACT_HANDPUSH  8
#define ACT_CAREFULLY 9
#define ACT_SHUTDOWN  10
#define ACT_AWAY      11
#define ACT_FAIL      12

typedef struct {
  int  act;
  bool connected;
} Pet;

Pet *pet;

void initialize(Pet *pet);  // 전원 켜기

bool isConnected(Pet *pet); // 블루투스 연결 상태 확인
bool isPlaying(Pet *pet);   // 특정 동작 중인지 확인

int connect(Pet *pet);      // 첫 연결

int bark(Pet *pet);         // 짖기
int sit(Pet *pet);          // 앉기
int call(Pet *pet);         // 호출
int back(Pet *pet);         // 후퇴
int hold(Pet *pet);         // 정지
int walk(Pet *pet);         // 산책
int turn(Pet *pet);         // 회전
int handpush(Pet *pet);     // 왼 앞발 내밀기
int carefully(Pet *pet);    // 귀 쫑긋
int shutdown(Pet *pet);     // 전원 끄기
int away(Pet *pet);         // 거리 이탈 방지
int fail(Pet *pet);         // 명령 해석 실패

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

  pet = (Pet *)malloc(sizeof(Pet));

  initialize(pet); // RQ-DEV-07(로봇 펫 전원 켜기)
}

void loop()
{
  /*
   * TODO: implement.
   * 
   * 배터리 잔량 확인 함수, 방전 방지 함수, 충돌방지 함수구현
   */
  
  if (Serial1.available()) // 블루투스 메세지 수신
  {
    int message = Serial.read();
    int return_code = 0;
    
    if (!isConnected(pet)) // RQ-DEV-09(블루투스 연결 알림): 로봇 펫은 왼쪽 귀와 오른쪽 귀를 세우고 꼬리를 흔들며 한번 짖는다.
    {
      return_code = connect(pet);
    }
    else                   // 블루투스는 이미 연결돼 있었음.
    {
      switch(message - ACT_CODE) {
        case ACT_BARK: // RQ-USR-01(로봇 펫 호명): 로봇 펫이 짖는다.
          return_code = bark(pet);
          break;
          
        case ACT_SIT:  // RQ-USR-02(로봇 펫 앉아): 로봇 펫이 제자리에 앉는다.
          return_code = sit(pet);
          break;
          
        case ACT_CALL: // RQ-USR-03(로봇 펫 호출): 로봇 펫이 사용자 방향으로 이동한다.
          return_code = call(pet);
          break;
          
        case ACT_BACK: // RQ-USR-04(로봇 펫 후퇴): 로봇 펫이 사용자와 먼 방향으로 이동한다.
          return_code = back(pet);
          break;
          
        case ACT_HOLD: // RQ-USR-05(로봇 펫 정지): 로봇 펫이 현재 행동을 멈추고 가만히 서 있는다.
          return_code = hold(pet);
          break;
          
        case ACT_WALK: // RQ-USR-06(로봇 펫 산책): 로봇 펫이 귀와 꼬리를 2초간 흔든다.
          return_code = walk(pet);
          break;
          
        case ACT_TURN: // RQ-USR-07(로봇 펫 회전): 로봇 펫이 제자리에서 시계방향으로 1바퀴 회전한다.
          return_code = turn(pet);
          break;
          
        case ACT_HANDPUSH: // RQ-USR-08(로봇 펫 손): 로봇 펫이 왼 앞발을 내민다.
          return_code = handpush(pet);
          break;
          
        case ACT_CAREFULLY: // RQ-USR-09(로봇 펫 쫑긋): 로봇 펫이 왼쪽 귀와 오른쪽 귀를 각각 1초씩 접었다 편다.
          return_code = carefully(pet);
          break;

        case ACT_SHUTDOWN:  // RQ-DEV-08(로봇 펫 전원 끄기): 로봇 펫이 왼쪽 귀와 오른쪽 귀, 꼬리를 내리고 제자리에 앉는다.
          return_code = shutdown(pet);
          break;
        
        case ACT_AWAY:      // RQ-DEV-11(로봇 펫 거리 이탈 방지): 로봇 펫이 현재 행동을 멈추고 가만히 서 있는다.
          return_code = away(pet);
          break;
          
        default:  // RQ-DEV-10(명령 해석 실패): 로봇 펫이 왼쪽 귀와 오른쪽 귀, 꼬리를 내린다.
          return_code = fail(pet);
          break;
      }
    }

    switch (return_code) {
      case RC_OK:      // 정상 종료
        break;
        
      case RC_PLAYING: // 중복 동작 에러 코드(RQ-DEV-03)
        /*
         * TODO: implement.
         * 
         * 에러 핸들링.
         */
        break;
        
      default:
        break;
    }
  }
}

void initialize(Pet *pet)
{
  pet->act = ACT_INIT;
  
  /*
   * TODO: implement.
   * 
   * 가만히 서서 왼쪽 귀와 오른 쪽 귀를 세우고 꼬리를 내림.
   */
   
  pet->act = ACT_NONE;
  pet->connected = false;
}

bool isConnected(Pet *pet)
{
  return pet->connected;
}

bool isPlaying(Pet *pet)
{
  return pet->act;
}

int connect(Pet *pet)
{
  pet->act = ACT_CONNECT;
  
  /*
   * TODO: implement.
   * 
   * 왼쪽 귀와 오른쪽 귀를 세우고 꼬리를 흔들기.
   */

  while (bark(pet) != RC_OK); // 한번 짖음.
  
  pet->connected = true;
  
  pet->act = ACT_NONE;
  return RC_OK;
}

int bark(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;
    
  pet->act = ACT_BARK;
  
  /*
   * TODO: implement.
   * 
   * 짖기.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int sit(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;
    
  pet->act = ACT_SIT;
  
  /*
   * TODO: implement.
   * 
   * 제자리에 앉기.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int call(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;
    
  pet->act = ACT_CALL;
  
  /*
   * TODO: implement.
   * 
   * 사용자 방향으로 이동.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int back(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;
    
  pet->act = ACT_BACK;
  
  /*
   * TODO: implement.
   * 
   * 사용자와 먼 방향으로 이동.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int hold(Pet *pet)
{
  pet->act = ACT_HOLD;
  
  initialize(pet); // 현재 행동을 멈추고 정지.

  pet->act = ACT_NONE;
  return RC_OK;
}

int walk(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;

  pet->act = ACT_WALK;
  
  /*
   * TODO: implement.
   * 
   * 귀와 꼬리를 2초간 흔듬.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int turn(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;

  pet->act = ACT_TURN;
  
  /*
   * TODO: implement.
   * 
   * 제자리에서 1바퀴 시계방향으로 회전.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int handpush(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;

  pet->act = ACT_HANDPUSH;

  /*
   * TODO: implement.
   * 
   * 왼 앞발을 내밈.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int carefully(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;

  pet->act = ACT_CAREFULLY;

  /*
   * TODO: implement.
   * 
   * 왼쪽 귀와 오른쪽 귀를 각각 1초씩 접었다 핌.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}

int shutdown(Pet *pet)
{
  int return_code = 0;

  // 왼쪽 귀와 오른 쪽 귀, 꼬리를 내리고 제자리에 앉는다.
  while (fail(pet) != RC_OK); // 왼쪽 귀와 오른 쪽 귀, 꼬리를 내림.
  while (sit(pet) != RC_OK);  // 제자리에 앉음.

  pet->act = ACT_SHUTDOWN;
  pet->connected = false;

  return RC_OK;
}

int away(Pet *pet)
{
  pet->act = ACT_AWAY;
  hold(pet); // 현재 행동을 멈추고 정지.
  pet->act = ACT_NONE;
  return RC_OK;
}

int fail(Pet *pet)
{
  if (isPlaying(pet))
    return RC_PLAYING;

  pet->act = ACT_FAIL;

  /*
   * TODO: implement.
   * 
   * 왼쪽 귀와 오른쪽 귀, 꼬리를 내림.
   */

  pet->act = ACT_NONE;
  return RC_OK;
}
