/********************************************************************\
 * recncell.c -- reconcile checkbox cell                            *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

/*
 * FILE:
 * recncell.c
 * 
 * FUNCTION:
 * Implements a mouse-click cell that allows a series
 * of values to be clicked through.
 *
 * HISTORY:
 * Copyright (c) 1998 Linas Vepstas
 * Copyright (c) 2000 Dave Peticolas
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "basiccell.h"
#include "gnc-engine-util.h"
#include "recncell.h"

/* hack alert -- I am uncomfortable with including engine
 * stuff here; all code in this directory should really be 
 * independent of the engine.  Its just that we need the
 * defs for YREC, CREC, etc. This is some cleanup we should 
 * do some day.
 */
#include "Transaction.h"

static RecnCellStringGetter string_getter = NULL;

/* This static indicates the debugging module that this .o belongs to.  */
static short module = MOD_REGISTER;

static void gnc_recn_cell_set_value (BasicCell *_cell, const char *value);


static const char *
gnc_recn_cell_get_string (char reconciled_flag)
{
  static char str[2] = { 0, 0 };

  if (string_getter != NULL)
    return (string_getter)(reconciled_flag);

  str[0] = reconciled_flag;

  return str;
}

static gboolean
gnc_recn_cell_enter (BasicCell *_cell,
                     int *cursor_position,
                     int *start_selection,
                     int *end_selection)
{
  RecnCell *cell = (RecnCell *) _cell;

  if (cell->confirm_cb &&
      ! (cell->confirm_cb (cell->reconciled_flag, cell->confirm_data)))
    return FALSE;

  if (cell->reconciled_flag == NREC)
    cell->reconciled_flag = CREC;
  else
    cell->reconciled_flag = NREC;

  gnc_recn_cell_set_flag (cell, cell->reconciled_flag);

  return FALSE;
}

static void
gnc_recn_cell_init (RecnCell *cell)
{
  gnc_basic_cell_init (&cell->cell);

  gnc_recn_cell_set_flag (cell, NREC);
  cell->confirm_cb = NULL;

  cell->cell.enter_cell = gnc_recn_cell_enter;
  cell->cell.set_value = gnc_recn_cell_set_value;
}

BasicCell *
gnc_recn_cell_new (void)
{
  RecnCell * cell;

  cell = g_new0 (RecnCell, 1);

  gnc_recn_cell_init (cell);

  return &cell->cell;
}

/* assumes we are given the untranslated form */
static void
gnc_recn_cell_set_value (BasicCell *_cell, const char *value)
{
  RecnCell *cell = (RecnCell *) _cell;
  char flag;

  if (!value || *value == '\0')
  {
    cell->reconciled_flag = NREC;
    gnc_basic_cell_set_value_internal (_cell, "");
    return;
  }

  switch (*value)
  {
    case NREC:
    case CREC:
    case YREC:
    case FREC:
      flag = *value;
      break;

    default:
      PWARN ("unexpected recn flag");
      flag = NREC;
      break;
  }

  gnc_recn_cell_set_flag (cell, flag);
}

void
gnc_recn_cell_set_flag (RecnCell *cell, char reconciled_flag)
{
  const char *string;

  g_return_if_fail (cell != NULL);

  cell->reconciled_flag = reconciled_flag;

  string = gnc_recn_cell_get_string (reconciled_flag);

  gnc_basic_cell_set_value_internal (&cell->cell, string);
}

char
gnc_recn_cell_get_flag (RecnCell *cell)
{
  g_return_val_if_fail (cell != NULL, NREC);

  return cell->reconciled_flag;
}

void
gnc_recn_cell_set_string_getter (RecnCellStringGetter getter)
{
  string_getter = getter;
}

void
gnc_recn_cell_set_confirm_cb (RecnCell *cell, RecnCellConfirm confirm_cb,
                              gpointer data)
{
  g_return_if_fail (cell != NULL);

  cell->confirm_cb = confirm_cb;
  cell->confirm_data = data;
}