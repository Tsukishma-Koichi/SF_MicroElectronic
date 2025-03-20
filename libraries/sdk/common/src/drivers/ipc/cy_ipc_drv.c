/***************************************************************************//**
* \file cy_ipc_drv.c
* \version 1.10
*
*  \breif
*   IPC Driver - This source file contains the low-level driver code for
*   the IPC hardware.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_ipc_drv.h"


/*******************************************************************************
* Function Name: Cy_IPC_Drv_LockAcquire
****************************************************************************//**
*
* This function is used to acquire the IPC channel.
*
* This function is internal and should not be called directly by user software
*
* \param base
* This parameter is a handle that represents the base address of the registers
* of the IPC channel.
* The parameter is generally returned from a call to the \ref
* Cy_IPC_Drv_GetIpcBaseAddress
*
* \return   Status of the operation
*   \retval CY_IPC_DRV_SUCCESS: The IPC was successfully acquired
*   \retval CY_IPC_DRV_ERROR:   The IPC was not acquired because it was already acquired
*                       by another master
*
*******************************************************************************/
cy_en_ipcdrv_status_t Cy_IPC_Drv_LockAcquire (volatile stc_IPC_STRUCT_t const * base)
{
    cy_en_ipcdrv_status_t retStatus;

    if( 0ul != base->unACQUIRE.stcField.u1SUCCESS )
    {
        retStatus = CY_IPC_DRV_SUCCESS;
    }
    else
    {
        retStatus = CY_IPC_DRV_ERROR;
    }
    return(retStatus);
}

/*******************************************************************************
* Function Name: Cy_IPC_Drv_LockRelease
****************************************************************************//**
*
* The function is used to release an IPC channel from the locked state.
* The function also has a way to specify through a parameter which IPC
* interrupts must be notified during the release event.
*
* This function is internal and should not be called directly by user software.
*
* \param base
* This parameter is a handle that represents the base address of the registers
* of the IPC channel.
* The parameter is generally returned from a call to the \ref
* Cy_IPC_Drv_GetIpcBaseAddress.
*
* \param releaseEventIntr
* Bit encoded list of IPC interrupt lines that are triggered by a release event.
*
* \return   Status of the operation
*   \retval CY_IPC_DRV_SUCCESS: The function executed successfully and the IPC channel
*                       was released.
*   \retval CY_IPC_DRV_ERROR: The IPC channel was not acquired before the
*                       function call.
*
*******************************************************************************/
cy_en_ipcdrv_status_t  Cy_IPC_Drv_LockRelease (volatile stc_IPC_STRUCT_t* base, uint32_t releaseEventIntr)
{
    cy_en_ipcdrv_status_t retStatus;

   /* Check to make sure the IPC is Acquired */
   if( Cy_IPC_Drv_IsLockAcquired(base) )
   {
        /* The IPC was acquired, release the IPC channel */
        Cy_IPC_Drv_ReleaseNotify(base, releaseEventIntr);

        retStatus = CY_IPC_DRV_SUCCESS;
    }
    else   /* The IPC channel was already released (not acquired) */
    {
        retStatus = CY_IPC_DRV_ERROR;
    }

    return(retStatus);
}

/*******************************************************************************
* Function Name: Cy_IPC_Drv_SendMsgWord
****************************************************************************//**
*
* This function is used to send a 32-bit word message through an IPC channel.
* The function also has an associated notification field that will let the
* message notify one or multiple IPC interrupts. The IPC channel is locked and
* remains locked after the function returns.  The receiver of the message should
* release the channel.
*
* This function is internal and should not be called directly by user software.
*
* \param base
* This parameter is a handle that represents the base address of the registers
* of the IPC channel.
* The parameter is generally returned from a call to the \ref
* Cy_IPC_Drv_GetIpcBaseAddress.
*
* \param notifyEventIntr
* Bit encoded list of IPC interrupt lines that are triggered by a notification.
*
* \param message
* The message word that is the data placed in the IPC data register.
*
* \return   Status of the operation:
*   \retval CY_IPC_DRV_SUCCESS: The send operation was successful.
*   \retval CY_IPC_DRV_ERROR: The IPC channel is unavailable because it is already locked.
*
*******************************************************************************/
cy_en_ipcdrv_status_t  Cy_IPC_Drv_SendMsgWord (volatile stc_IPC_STRUCT_t* base, uint32_t notifyEventIntr, uint32_t message)
{
    cy_en_ipcdrv_status_t retStatus;

    if( CY_IPC_DRV_SUCCESS == Cy_IPC_Drv_LockAcquire(base) )
    {
        /* If the channel was acquired, send the message. */
        Cy_IPC_Drv_WriteDataValue(base, message);

        Cy_IPC_Drv_AcquireNotify(base, notifyEventIntr);

        retStatus = CY_IPC_DRV_SUCCESS;
    }
    else
    {
        /* Channel was already acquired, return Error */
        retStatus = CY_IPC_DRV_ERROR;
    }
    return(retStatus);
}

/*******************************************************************************
* Function Name: Cy_IPC_Drv_SendMsgWord_2
****************************************************************************//**
*
* This function is used to send two 32-bit word message through an IPC channel.
* The function also has an associated notification field that will let the
* message notify one or multiple IPC interrupts. The IPC channel is locked and
* remains locked after the function returns.  The receiver of the message should
* release the channel.
*
* This function is internal and should not be called directly by user software.
*
* \param base
* This parameter is a handle that represents the base address of the registers
* of the IPC channel.
* The parameter is generally returned from a call to the \ref
* Cy_IPC_Drv_GetIpcBaseAddress.
*
* \param notifyEventIntr
* Bit encoded list of IPC interrupt lines that are triggered by a notification.
*
* \param message
* The message word that is the data placed in the IPC data register.

* \param message2
* The message word that is the data placed in the IPC data1 register.
*
* \return   Status of the operation:
*   \retval CY_IPC_DRV_SUCCESS: The send operation was successful.
*   \retval CY_IPC_DRV_ERROR: The IPC channel is unavailable because it is already locked.
*
*******************************************************************************/
cy_en_ipcdrv_status_t  Cy_IPC_Drv_SendMsgWord_2 (volatile stc_IPC_STRUCT_t* base, uint32_t notifyEventIntr, uint32_t message, uint32_t message2)
{
    cy_en_ipcdrv_status_t retStatus;

    if( CY_IPC_DRV_SUCCESS == Cy_IPC_Drv_LockAcquire(base) )
    {
        /* If the channel was acquired, send the message. */
        Cy_IPC_Drv_WriteDataValue(base, message);
        Cy_IPC_Drv_WriteData1Value(base, message2);

        Cy_IPC_Drv_AcquireNotify(base, notifyEventIntr);

        retStatus = CY_IPC_DRV_SUCCESS;
    }
    else
    {
        /* Channel was already acquired, return Error */
        retStatus = CY_IPC_DRV_ERROR;
    }
    return(retStatus);
}


/*******************************************************************************
* Function Name: Cy_IPC_Drv_ReadMsgWord
****************************************************************************//**
*
* This function is used to read a 32-bit word message through an IPC channel.
* This function assumes that the channel is locked (for a valid message).
* If the channel is not locked, the message is invalid.  The user must call
* Cy_IPC_Drv_Release() function after reading the message to release the
* IPC channel.
*
* This function is internal and should not be called directly by user software.
*
* \param base
* This parameter is a handle that represents the base address of the registers
* of the IPC channel.
* The parameter is generally returned from a call to the \ref
* Cy_IPC_Drv_GetIpcBaseAddress.
*
* \param message
*  A variable where the read data is copied.
*
* \return  Status of the operation
*   \retval CY_IPC_DRV_SUCCESS: The function executed successfully and the IPC
*                       was acquired.
*   \retval CY_IPC_DRV_ERROR:   The function encountered an error because the IPC
*                       channel was already in a released state, meaning the data
*                       may be invalid.
*
*******************************************************************************/
cy_en_ipcdrv_status_t  Cy_IPC_Drv_ReadMsgWord (volatile stc_IPC_STRUCT_t const * base, uint32_t * message)
{
    cy_en_ipcdrv_status_t retStatus;

    if ( Cy_IPC_Drv_IsLockAcquired(base) )
    {
        /* The channel is locked; message is valid. */
        *message = Cy_IPC_Drv_ReadDataValue(base);

        retStatus = CY_IPC_DRV_SUCCESS;
    }
    else
    {
        /* The channel is not locked so channel is invalid. */
        retStatus = CY_IPC_DRV_ERROR;
    }
    return(retStatus);
}

/*******************************************************************************
* Function Name: Cy_IPC_Drv_ReadMsgWord_2
****************************************************************************//**
*
* This function is used to read two 32-bit word messages through an IPC channel.
* This function assumes that the channel is locked (for a valid message).
* If the channel is not locked, the message is invalid.  The user must call
* Cy_IPC_Drv_Release() function after reading the message to release the
* IPC channel.
*
* This function is internal and should not be called directly by user software.
*
* \param base
* This parameter is a handle that represents the base address of the registers
* of the IPC channel.
* The parameter is generally returned from a call to the \ref
* Cy_IPC_Drv_GetIpcBaseAddress.
*
* \param message
*  A variable where the read data is copied.
*
* \param message
*  A variable where the read data1 is copied.
*
* \return  Status of the operation
*   \retval CY_IPC_DRV_SUCCESS: The function executed successfully and the IPC
*                       was acquired.
*   \retval CY_IPC_DRV_ERROR:   The function encountered an error because the IPC
*                       channel was already in a released state, meaning the data
*                       may be invalid.
*
*******************************************************************************/
cy_en_ipcdrv_status_t  Cy_IPC_Drv_ReadMsgWord_2 (volatile stc_IPC_STRUCT_t const * base, uint32_t * message, uint32_t* message2)
{
    cy_en_ipcdrv_status_t retStatus;

    if ( Cy_IPC_Drv_IsLockAcquired(base) )
    {
        /* The channel is locked; message is valid. */
        *message  = Cy_IPC_Drv_ReadDataValue(base);
        *message2 = Cy_IPC_Drv_ReadData1Value(base);

        retStatus = CY_IPC_DRV_SUCCESS;
    }
    else
    {
        /* The channel is not locked so channel is invalid. */
        retStatus = CY_IPC_DRV_ERROR;
    }
    return(retStatus);
}


/* [] END OF FILE */
