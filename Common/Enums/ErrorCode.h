#pragma once
/// <summary>
    /// ��ö�ٶ����˷��������ܳ��ֵĸ��ִ����룬����ͳһ�����ʶ��ͬ���͵Ĵ���
    /// </summary>
enum ErrorCode
{
    /// <summary>
    /// �����ɹ���û�г����κδ���
    /// </summary>
    Success = 0x0000,

    // ������ش���
    /// <summary>
    /// ��������ʱ�������������粻�ȶ�����������Ӧ������ԭ���¡�
    /// </summary>
    NetworkTimeout = 0x1001,
    /// <summary>
    /// �޷����ӵ����ݿ⣬���������ݿ����δ�����������ַ������ô����ԭ��
    /// </summary>
    DatabaseConnectionFailed = 0x1002,
    /// <summary>
    /// �������޷����ʣ������Ƿ�����δ������������ϻ��������ַ����
    /// </summary>
    ServerUnreachable = 0x1003,
    /// <summary>
    /// �׽��ֲ������ִ����������ӱ��ܾ������Ӷ�ʧ�ȡ�
    /// </summary>
    SocketError = 0x1004,
    /// <summary>
    /// DNS ����ʧ�ܣ��޷�������ת��Ϊ IP ��ַ��
    /// </summary>
    DNSResolutionFailed = 0x1005,

    // ������֤����
    /// <summary>
    /// ��������ݲ�����Ҫ�󣬿������������͡����ȵȲ����Ϲ涨��
    /// </summary>
    InvalidInput = 0x2001,
    /// <summary>
    /// ������ȱ�ٱ�Ҫ�Ĳ����������޷���ɲ�����
    /// </summary>
    MissingRequiredParameter = 0x2002,
    /// <summary>
    /// ��������ڸ�ʽ����ȷ���޷���ϵͳ��ȷ������
    /// </summary>
    InvalidDateFormat = 0x2003,
    /// <summary>
    /// ����ĵ����ʼ���ַ��ʽ�����ϱ�׼��
    /// </summary>
    InvalidEmailFormat = 0x2004,
    /// <summary>
    /// ����ĵ绰�����ʽ����ȷ��
    /// </summary>
    InvalidPhoneNumber = 0x2005,
    /// <summary>
    /// ��������벻��������ǿ��Ҫ������������벻ƥ�䡣
    /// </summary>
    InvalidPassword = 0x2006,
    /// <summary>
    /// ����� JSON ���ݸ�ʽ����ȷ���޷���ϵͳ��ȷ������
    /// </summary>
    InvalidJSONFormat = 0x2007,
    /// <summary>
    /// ����� XML ���ݸ�ʽ����ȷ���޷���ϵͳ��ȷ������
    /// </summary>
    InvalidXMLFormat = 0x2008,

    // Ȩ�޴���
    /// <summary>
    /// �û�δ������Ȩ�����Է�����ҪȨ�޵���Դ��ִ����ҪȨ�޵Ĳ�����
    /// </summary>
    UnauthorizedAccess = 0x3001,
    /// <summary>
    /// �û���Ȩ�޲��㣬�޷����������Ĳ�����
    /// </summary>
    InsufficientPermissions = 0x3002,
    /// <summary>
    /// �û��������ѹ��ڣ���Ҫ���»�ȡ��Ч�����ơ�
    /// </summary>
    TokenExpired = 0x3003,
    /// <summary>
    /// �û��ṩ��������Ч�����������Ʊ��۸Ļ��ʽ����
    /// </summary>
    TokenInvalid = 0x3004,
    /// <summary>
    /// �û��˻������������������ڶ�γ��Ե�¼ʧ�ܵ�ԭ��
    /// </summary>
    AccountLocked = 0x3005,

    // �������ڲ�����
    /// <summary>
    /// �������ڲ�����δ֪���󣬿����Ǵ����߼��������ô����ԭ��
    /// </summary>
    InternalServerError = 0x5001,
    /// <summary>
    /// ��������Դ�ľ��������ڴ桢CPU ����Դ���㡣
    /// </summary>
    ResourceExhausted = 0x5002,
    /// <summary>
    /// �������ڴ治�㣬�޷������㹻���ڴ�����ɲ�����
    /// </summary>
    OutOfMemory = 0x5003,
    /// <summary>
    /// ������ļ������ڣ��������ļ���ɾ����·������
    /// </summary>
    FileNotFound = 0x5004,
    /// <summary>
    /// �����Ŀ¼�����ڣ�������Ŀ¼��ɾ����·������
    /// </summary>
    DirectoryNotFound = 0x5005,
    /// <summary>
    /// ���ݿ��ѯ�������ִ��󣬿����� SQL ���������ݿ��ṹ�����ԭ��
    /// </summary>
    DatabaseQueryError = 0x5006,
    /// <summary>
    /// ���ݿ���²���ʧ�ܣ����������ݳ�ͻ��Ȩ�޲����ԭ��
    /// </summary>
    DatabaseUpdateFailed = 0x5007,
    /// <summary>
    /// ���ݿ�������ʧ�ܣ������������ظ����ֶ�Լ����ԭ��
    /// </summary>
    DatabaseInsertFailed = 0x5008,
    /// <summary>
    /// ���ݿ�ɾ������ʧ�ܣ����������ݹ�����Ȩ�޲����ԭ��
    /// </summary>
    DatabaseDeleteFailed = 0x5009,

    // ҵ���߼�����
    /// <summary>
    /// ������û������ڣ��������û� ID ������û��ѱ�ɾ����
    /// </summary>
    UserNotFound = 0x6001,
    /// <summary>
    /// ����Ĳ�Ʒ�����ڣ������ǲ�Ʒ ID ������Ʒ���¼ܡ�
    /// </summary>
    ProductNotFound = 0x6002,
    /// <summary>
    /// ����Ķ��������ڣ������Ƕ��� ID ����򶩵��ѱ�ȡ����
    /// </summary>
    OrderNotFound = 0x6003,
    /// <summary>
    /// ����ʹ�õ��û����ѱ������û�ʹ�ã��������ظ���
    /// </summary>
    DuplicateUsername = 0x6004,
    /// <summary>
    /// ����ʹ�õĵ����ʼ���ַ�ѱ������û�ע�ᣬ�������ظ���
    /// </summary>
    DuplicateEmail = 0x6005,
    /// <summary>
    /// ��Ʒ��治�㣬�޷���ɶ�����
    /// </summary>
    InsufficientStock = 0x6006,
    /// <summary>
    /// ֧������ʧ�ܣ�������֧����ʽ�����˻������ԭ��
    /// </summary>
    PaymentFailed = 0x6007,
    /// <summary>
    /// ʹ�õ��Ż�ȯ�ѹ��ڣ��޷�����ʹ�á�
    /// </summary>
    CouponExpired = 0x6008,
    /// <summary>
    /// ʹ�õ��Ż�ȯ��Ч���������Ż�ȯ���������ѱ�ʹ�á�
    /// </summary>
    CouponInvalid = 0x6009
};
