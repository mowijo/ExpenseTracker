<?php

abstract class Error
{
    const BrokenJsonFromServer = 257;
    const MissingXCPIattributesHeader = 258;
    const MissingXCPIMtimeHeader = 259;
    const CannotUpdateADirectoryWithAFile = 260;
    const CannotUpdateAFileAsADirectory = 261;
    const CannotDeleteRoofOfAccount = 262;
    const CannotDeleteNonEmtyDirectory = 263;
    const CannotCreateAFileInAFile = 264;
    const FolderNotFoundCannotPutOntoIt = 265;
    const CredentialsNeeded = 266;
    const CredentialsMismatch = 267;
    const MethodNotImplemented = 268;
    const PUTDataBadFormat = 269;
    const CouldNotWriteFSEToDataStream = 513;
    const CouldNotReadFSEFromDataStream = 514;
    const CorruptedJournalChecksumRead = 515;
    const UnsupportedJournalVersion = 516;
    const CouldNotOpenJournalFileForWriting = 517;
    const CouldNotOpenJournalFileForReading = 518;
    const RootFolderDoesNotExist = 519;
    const RootFolderNotWritable = 520;
    const CannotCreateCacheFolderInRootFolder = 521;
    const CannotWriteToCacheFolder = 522;
    const CacheFolderIsNotFolder = 523;
}?>