/******************************************************************************************
 * Computer Science Note Source code
 * Javious Tian, javioustlj@outlook.com
 * 夏田墨  javious_tian@zuaa.zju.edu.cn
 * Copyright (c) 2024 Javioustlj. All rights reserved.
 ******************************************************************************************/

/******************************************************************************************
 * 同花顺面试题目
 * 如何优化这段代码？
 * 可读性
 * 可扩展性
 * 封装
 ******************************************************************************************/

//扫描用户列表
int32 CHqServer::UserScanProc()
{
	if(!IsRunning())
		return 0;


	// 设置暂停服务后一段时间(默认15s)后断开所有用户链接
	if(!m_ServerStatus.IsServiceEnable())
	{
		if(m_ServerStatus.IsNeedCloseSession(m_setting.m_nCloseUserDelay))
		{
			// 主动断开所有用户连接
			// 1.触发老客户端断开当前连接，连接下个站点
			// 2.新客户端15s内应该已经做了服务切换，会主动断开，如果没断开，服务端这边主动断开
			CloseUserSession(NULL);
			m_ServerStatus.ResetStopTime();
		}

		return 0;
	}

	int32 nRet = 0;
	//
	uint32 nOnline = 0;				//总用户数
	uint32 nHqOnline = 0;			// 行情连接
	uint32 nFlashOnline = 0;			// flash行情连接
	uint32 nTextOnline = 0;			// 资讯连接
	uint32 nThsTextOnline = 0;		// 同花顺资讯
	uint32 nRegisterOnline = 0;		//注册用户
	uint32 nThsOnline = 0;			//同花顺用户
	uint32 nDownloadOnline = 0;		//下载,升级用户
	uint32 nUpdate = 0;				//升级用户
	uint32 nDownload = 0;			//下载用户
	double dDownloadMemory = 0;		//下载内存
	double dUpdateMemory = 0;		//升级内存
	CSingleLock lockUser(&m_synUser);	
	lockUser.Lock();
	//统计在线人数
	nOnline = m_nOnlineSize;//m_lsOnlineUser.size();
	m_ServerStatus.m_sStatInfo.m_lNowCount = nOnline;
	//复制一份用户列表
	user_vector vtForScan;
	vtForScan.resize(nOnline);
	copy(m_lsOnlineUser.begin(), m_lsOnlineUser.end(), vtForScan.begin());

	time_t tmNow = baratol::TL_GetClockSecond();
	if(CanRebuildPushCache(tmNow))
	{
		m_lsFreshUser.clear();
	}
	lockUser.Unlock();
	
	//
	//控制每用户在线总数
	m_userStat.ResetAccount();

	uint32 uUserBlockCnt = 0;
	uint32 uRsBlockCnt = 0;
	int32 nRedUser_Push = 0;
	int32 nRedUser_Req = 0;
	int32 nRedUser_Reqdrop = 0;
	int32 nYellowUser_Push = 0;
	int32 nYellowUser_Req = 0;
	int32 nYellowUser_Reqdrop = 0;
	int32 nBlockCount[2];
	time_t tmLastSendTime = 0;
	user_vector vtUpdate;
	user_vector vtDownload;

	int32 nValidOnlineSize = 0;
	int32 nMaxOnlineSize = m_setting.m_nServerOnlineLimit;

	//按用户循环
	user_vector::iterator iter;
	CString strLogErrorMsg;
	for(iter = vtForScan.begin(); iter != vtForScan.end(); ++iter)
	{
		RefUserSession Session = *iter;
		if (pSession == NULL)
		{
			continue;
		}	

		//检查用户状态
		if(pSession->OnTimer(m_lLogKickUser, tmNow, TRUE) != SESSION_CONTINUE)
		{
			//需要再检测下并发是否上限否则踢掉
			MaxOnlineToKick(pSession, nMaxOnlineSize, nValidOnlineSize);
			continue;
		}
		else
		{
			if(MaxOnlineToKick(pSession, nMaxOnlineSize, nValidOnlineSize))
			{
				continue;
			}
		}

		CUserProperty* pUserProperty = pSession->GetUserProperty();

		//检查阻塞状态
		nBlockCount[0] = nBlockCount[1] = 0;
		pSession->SetLinkOptions(TL_NET_OPER_QUERY_SENDLIST, (integer)nBlockCount);
		tmLastSendTime = pSession->GetLastSendTime();
		//升级连接
		if(pUserProperty->GetRequestType() == SERVER_UPDATE)
		{
			++nUpdate;
			vtUpdate.push_back(pSession);
			dUpdateMemory += nBlockCount[0]*1.0/(1024*1024);
		}
		else if(pUserProperty->GetRequestType() == SERVER_SINGLE_DOWNLOAD)// 下载连接
		{
			++nDownload;
			vtDownload.push_back(pSession);
			dDownloadMemory += nBlockCount[0]*1.0/(1024*1024);
		}

		// 统计每用户在线
		CString strAccount;
		int nMaxOnline = pUserProperty->GetMaxOnline(strAccount);
		if (nMaxOnline > 0)
		{
			m_userStat.AddUserStatInfo(strAccount.c_str(), pSession, nMaxOnline);
		}
		//注册用户
		if(pUserProperty->IsRegisterClient())
		{
			++ nRegisterOnline;
		}
		uint64 dwUserType = pUserProperty->GetSessionOps(ULN_TYPE_MASK);
		//行情连接
		if(dwUserType & ULN_TYPE_HQ)
		{
#ifdef CALC_DATA_SOURCE
			//calcnode连接datasource,可能会导致cs堵包数较多, 
			//这里监控一下是否会有连续异常的堵包数产生,根据测试情况来看
			//目前堵包最严重大概为1200包,这里如果发现连续5次堵包超过1000
			//那么就打印一条日志记录一下
#define BLOCK_WARN_COUNT 1000
#define BLOCK_CONTINUE_COUNT 5
			static int32 nBlockDataCount = 0;
			if(nBlockCount[1] > BLOCK_WARN_COUNT)
			{
				nBlockDataCount++;
			}
			else
			{
				nBlockDataCount = 0;
			}

			if(nBlockDataCount >= BLOCK_CONTINUE_COUNT)
			{
				nBlockDataCount = 0;
				TL_LevelLogEventV(LOG_LEV_WARNING, "CHqServer::UserScanProc send queue block count more than %d continuous last %d time, a risk of hangqing delay", BLOCK_WARN_COUNT, BLOCK_CONTINUE_COUNT);
			}
#endif //CALC_DATA_SOURCE

			int32 nSendTimeOut = tmNow - tmLastSendTime;
			if((m_setting.m_nUserBlock > 0 && nBlockCount[1] > m_setting.m_nUserBlock)
				|| (m_setting.m_nUserTimeoutBlock > 0 && nBlockCount[1] > 0 && tmLastSendTime > 0 && nSendTimeOut > m_setting.m_nUserTimeoutBlock)	//当有堵包时检查堵包是否超时
				)
			{
				//断开阻塞的用户
				if (m_lLogBlockUser > 0) //记录断开用户信息的日志
				{
					CString strAccountName, strIp;
					pUserProperty->GetAccount(strAccountName);
					pUserProperty->GetRealIP(strIp);
					strLogErrorMsg.Format("kick user because of send blocked: username=%s, ip=%s, port=%d, userversion=%s, blockcnt=%u, blockcntlimit=%d, lastsendtime=%u, sendtimeout=%d, sendtimeoutlimit=%d"
						, (LPCTSTR)strAccountName, (LPCTSTR)strIp, pUserProperty->GetPeerPort(), pUserProperty->GetUserVersion(TRUE)
						, nBlockCount[1],  m_setting.m_nUserBlock, (uint32)tmLastSendTime, nSendTimeOut, m_setting.m_nUserTimeoutBlock);
					TL_LogEventV(m_lLogBlockUser,  (LPCTSTR)strLogErrorMsg);
				}
				
				strLogErrorMsg.Format("kick user because of send blocked: blockcnt=%u, lastsendtime=%u", nBlockCount[1], (uint32)tmLastSendTime);

				TL_InterlockedIncrement(&m_setting.m_nKickBlock);
				pSession->SetUserError(m_lLogKickUser, HIM_ERROR_BLOCK, strLogErrorMsg);
				continue;
			}

			++ nHqOnline;	

			if(pUserProperty->IsThsClient())
			{
				++ nThsOnline;
			}
			uUserBlockCnt += nBlockCount[1];

			//统计用户红黄码
			if(pUserProperty->IsPushDelayOverThreshold(m_setting.m_stRedUser_Push.m_nTimeThreshold, m_setting.m_stRedUser_Push.m_nCountThreshold))
			{
				++nRedUser_Push;
			}
			if(pUserProperty->IsPushDelayOverThreshold(m_setting.m_stYellowUser_Push.m_nTimeThreshold, m_setting.m_stYellowUser_Push.m_nCountThreshold))
			{
				++nYellowUser_Push;
			}
			if(pUserProperty->IsTcpReqOverThreshold(m_setting.m_stRedUser_Req.m_nTimeThreshold, m_setting.m_stRedUser_Req.m_nCountThreshold))
			{
				++nRedUser_Req;
			}
			if(pUserProperty->IsTcpReqOverThreshold(m_setting.m_stYellowUser_Req.m_nTimeThreshold, m_setting.m_stYellowUser_Req.m_nCountThreshold))
			{
				++nYellowUser_Req;
			}
			if(pUserProperty->IsTcpReqOverThreshold(m_setting.m_stRedUser_Reqdrop.m_nTimeThreshold, m_setting.m_stRedUser_Reqdrop.m_nCountThreshold))
			{
				++nRedUser_Reqdrop;
			}
			if(pUserProperty->IsTcpReqOverThreshold(m_setting.m_stYellowUser_Reqdrop.m_nTimeThreshold, m_setting.m_stYellowUser_Reqdrop.m_nCountThreshold))
			{
				++nYellowUser_Reqdrop;
			}
		}

		//资讯连接
		if (dwUserType & ULN_TYPE_TEXT)
		{
			++ nTextOnline;
			if (pUserProperty->IsThsClient())
			{
				++ nThsTextOnline;
			}
		}
		//flash客户端
		if(pUserProperty->IsFlashClient())
		{
			++nFlashOnline;
		}
		//下载、升级连接
		// modify by mojiayong  SRH_SESSION_DOWNLOAD这里的计数只记录了收到第一个请求包之后的连接， 10秒钟扫描一次会遗漏很多 ，因此根据requesttype判断
		//if(pUserProperty->GetOptions(SRH_SESSION_DOWNLOAD)) 
		if(pUserProperty->GetRequestType() == SERVER_DOWNLOAD)
		{
			++nDownloadOnline;
		}
		if(HIO_RECE_LINK == pUserProperty->GetUserOptions(HIO_RECE_LINK))
		{
			uRsBlockCnt += nBlockCount[1];
		}
		//升级连接
		//long pValue[2];
		//if(pUserProperty->GetRequestType() == SERVER_UPDATE)
		//{
		//	++nUpdate;
		//	vtUpdate.push_back(pSession);
		//	pSession->SetLinkOptions(TL_NET_OPER_QUERY_SENDLIST, (integer)pValue);
		//	nUpdateMemory += pValue[0];
		//}
		////下载连接
		//if(pUserProperty->GetRequestType() == SERVER_SINGLE_DOWNLOAD)
		//{
		//	++nDownload;
		//	vtDownload.push_back(pSession);
		//	pSession->SetLinkOptions(TL_NET_OPER_QUERY_SENDLIST, (integer)pValue);
		//	nDownloadMemory += pValue[0];
		//}
	}
#if defined __LINUX || _MSC_VER >= 1800
	HQStatsMetrics().SetMetricLinkBlock(prometheus::Type_Full, TYPE_UPCONN, 0, uRsBlockCnt);
	HQStatsMetrics().SetMetricLinkBlock(prometheus::Type_Full, TYPE_REMOTECONN, 0, uUserBlockCnt);

	HQStatsMetrics().SetMetricInfluence_PushUser(TYPE_PUSH, CHqServerConfig::Influence_Red, nRedUser_Push);
	HQStatsMetrics().SetMetricInfluence_ReqUser(TYPE_TCPREQ, CHqServerConfig::Influence_Red, nRedUser_Req);
	HQStatsMetrics().SetMetricInfluence_DropUser(TYPE_TCPREQ, CHqServerConfig::Influence_Red, nRedUser_Reqdrop);
	HQStatsMetrics().SetMetricInfluence_PushUser(TYPE_PUSH, CHqServerConfig::Influence_Yelow, nYellowUser_Push);
	HQStatsMetrics().SetMetricInfluence_ReqUser(TYPE_TCPREQ, CHqServerConfig::Influence_Yelow, nYellowUser_Req);
	HQStatsMetrics().SetMetricInfluence_DropUser(TYPE_TCPREQ, CHqServerConfig::Influence_Yelow, nYellowUser_Reqdrop);
#endif

	if (m_setting.m_bUserInfoDayLog)
	{// 上交所日志跨天处理
		time_t tmNow = baratol::TL_GetClockSecond() + USERSCAN_SPACE + 1;
		if (tmNow > m_tmUserInfoNextDay)
		{// 快进入第二天,对跨天的用户记录以0点分割记录两条
			TL_LogEventV("UserScanProc() newday log time: %d", (int32)tmNow);
			for(iter = vtForScan.begin(); iter != vtForScan.end(); ++iter)
			{
				RefUserSession pSession = *iter;
				if (pSession == NULL)
				{
					continue;
				}

				CUserProperty* pUserProperty = pSession->GetUserProperty();
				int32 nLoginTime = pUserProperty->GetLoginTime();// 防止登录时的时间超过当前的退出时间
				if (nLoginTime < m_tmUserInfoNextDay)				
				{
					LogUserTrack(pUserProperty, m_tmUserInfoNextDay-1, TRUE);
					pUserProperty->UpdateLoginTime(m_tmUserInfoNextDay);
				}
			}
			m_tmUserInfoNextDay = tmNow + 60*60*24;
		}
	}

	//控制每用户在线总数
	if (m_setting.m_bKickUserMax)
	{// Mod by chenjw 2014-05-26, 免费站点同个用户多个链接不踢人
		m_userStat.KickAccount(UOS_KICK_HEAD, fastdelegate::MakeDelegate(this, &CHqServer::KickAccount));
	}

	// 对升级,下载的带宽进行控制
	DWORD dwUpdateLimit = nUpdate ? (m_setting.m_dwUpdateBandWidth * 1024 * 1024/(nUpdate*8)) : 0;
	if(nUpdate && m_setting.m_dwUpdateLimit != dwUpdateLimit)
	{
		m_setting.m_dwUpdateLimit = dwUpdateLimit;
		SetBandWidth(vtUpdate, m_setting.m_dwUpdateLimit);
	}
	
	DWORD dwDownloadLimit = nDownload ? (m_setting.m_dwDownloadBandWidth * 1024 * 1024/(nDownload*8)) : 0;
	if(nDownload && m_setting.m_dwDownloadLimit != dwDownloadLimit)
	{
		m_setting.m_dwDownloadLimit = dwDownloadLimit;
		SetBandWidth(vtDownload, m_setting.m_dwDownloadLimit);
	}

	//检查连接队列
	CSingleLock lockConnect(&m_synConnect);
	lockConnect.Lock();
	user_list::iterator iterConnect = m_lsConnectLink.begin();
	for(; iterConnect != m_lsConnectLink.end(); )
	{
		RefUserSession pSession = *iterConnect;
		if (pSession == NULL)
		{
			continue;
		}
		DWORD dwLinkStatus = pSession->GetNetStatus();
		if(dwLinkStatus == TL_NET_STATUS_IDLE)
		{
			TLTrace("CHqServer::UserScanProc erase connect session\n");
			// 这里连接不能直接删除释放，会引起多线程对象被删除崩溃
			// 调用Close，通过baratol中socket回调到CHqServer::OnUserDisconnect进行释放
			//m_lsConnectLink.erase(iterConnect++);
			(*iterConnect)->Close(0);
			iterConnect++;
		}
		else
		{
			++iterConnect;
		}
	}
	lockConnect.Unlock();

	CSingleLock lockStatus(&m_synServerStatus, TRUE);
	//分类用户统计
	SetUserOnline(UMV_USER_REGISTER, nRegisterOnline);
	SetUserOnline(SERVER_HQ, nHqOnline);
	SetUserOnline(UMV_USER_THSHQ, nThsOnline);
	SetUserOnline(SERVER_TEXT, nTextOnline);
	SetUserOnline(UMV_USER_THSTEXT, nThsTextOnline);
	SetUserOnline(UMV_USER_FLASH, nFlashOnline);
	SetUserOnline(SERVER_DOWNLOAD, nDownloadOnline);
	SetUserOnline(SERVER_UPDATE, nUpdate);
	SetUserOnline(SERVER_SINGLE_DOWNLOAD, nDownload);
	// 统计当天最大行情以及最大资讯在线
	if(nHqOnline > m_ServerStatus.m_sStatInfo.m_lMaxHqOnline)
		m_ServerStatus.m_sStatInfo.m_lMaxHqOnline = nHqOnline;
	if(nTextOnline > m_ServerStatus.m_sStatInfo.m_lMaxTextOnline)
		m_ServerStatus.m_sStatInfo.m_lMaxTextOnline = nTextOnline;
	
    //内存占用设置
	SetMemorySize(MEMORY_UPDATE, (uint32)dUpdateMemory);
	SetMemorySize(MEMORY_DOWNLOAD, (uint32)dDownloadMemory);
	
	SetShowMonitorData();

	//统计最大在线
	if(nOnline > m_ServerStatus.m_sStatInfo.m_lMaxOnline)
		m_ServerStatus.m_sStatInfo.m_lMaxOnline = nOnline;
	SetUserOnline(SERVER_ALL, nOnline);

	//记录本站在线
	int32 lToday = TL_LongTimeToLongTime((int32)tmNow, UT_TRACE, UT_DAY, 0);
	m_trackUser.LogUserStat(lToday, nOnline, nHqOnline, nTextOnline);
	m_trackUser.GetStatCount(m_ServerStatus.m_sStatInfo.m_lTotalCount, m_ServerStatus.m_sStatInfo.m_lTodayCount);
	m_ServerStatus.m_sStatInfo.m_lTotalCount += nOnline;
	m_ServerStatus.m_sStatInfo.m_lTodayCount += nOnline;

	// 资讯包最后更新时间
	UpdateInfoDelay();

	lockStatus.Unlock();

	// 统计http连接
	time_t tMinUpdate = tmNow - _ttol(m_httpHandler.GetEnv(ENV_KEEP_ALIVE));
	typedef tl_vector::type HttpLinkContainer;
	HttpLinkContainer httplinks;
	CSingleLock lock(&m_synHttp, TRUE);
	size_t nHttpLinks = m_lsHttp.size();
	httplinks.resize(nHttpLinks);
	copy(m_lsHttp.begin(), m_lsHttp.end(), httplinks.begin());
	lock.Unlock();

	for (HttpLinkContainer::iterator iterHttp = httplinks.begin(); iterHttp != httplinks.end(); ++ iterHttp)
	{
		(*iterHttp)->TimerCheck(tMinUpdate);
	}

	return nRet;
}
