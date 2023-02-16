서버연동 프로그램 깃허브 업로드
간단 매뉴얼

KETI_PointTranslator/main.cpp
ClientConnect(QString IP) //분산시스템 연결, 여러개 가능
void gen_hologram() //분산시스템에 데이터 송수신 및 노말라이즈, 프린지패턴 이미지 출력
loadData(std::vector<fvec3>& point_data) //Color/Depth Map 불러와서 Point Cloud로 변환

holo_server/clientsocket.cpp
데이터 수신 및 프린지패턴 생성 후 통합 PC에 송신

시작 프로젝트로 설정
KETI_PointTranslator-통합 PC용 프로그램 생성(bin/KETI_PointTranslator.exe)
holo_server-분산 시스템용 프로그램 생성 (bin/holo_server.exe)

분산시스템에서 holo_server 실행 후 통합 PC에서 KETI_PointTranslator 실행하시면 됩니다.