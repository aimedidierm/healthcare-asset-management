<?php

namespace App\Http\Controllers\Api;

use App\Enums\RequestAction;
use App\Enums\StockMovementType;
use App\Http\Controllers\Controller;
use App\Http\Requests\HardwareRequest;
use App\Http\Requests\ItemStockRequest;
use App\Models\Hardware;
use App\Models\Item;
use App\Models\StockMovement;
use Illuminate\Support\Facades\Log;
use Symfony\Component\HttpFoundation\Response;

class HardwareController extends Controller
{
    public function show(HardwareRequest $request)
    {
        Log::info('This is an informational log message.' . $request);
        if ($request->has('temperature')) {

            Hardware::create([
                'temperature' => $request->input('temperature'),
                'humidity' => $request->input('humidity'),
            ]);

            return response()->json([
                'status' => "1",
                'message' => 'Data saved',
            ], Response::HTTP_CREATED);
        }

        if ($request->has('uid')) {

            Log::info('This is an informational log message.' . $request);
            if ($request->action == RequestAction::ADD->value) {
                $item = Item::where('uid', $request->input('uid'))->first();
                if ($item) {

                    StockMovement::create([
                        'item_id' => $item->id,
                        'quantity' => $item->qty,
                        'type' => StockMovementType::Addition->value,
                    ]);

                    return response()->json([
                        'status' => "1",
                        'message' => 'Item Added',
                    ], Response::HTTP_NOT_FOUND);
                } else {
                    return response()->json([
                        'status' => "0",
                        'message' => 'Item not found',
                    ], Response::HTTP_NOT_FOUND);
                }
            }

            if ($request->action == RequestAction::REMOVE->value) {
                $item = Item::where('uid', $request->input('uid'))->first();
                if ($item) {

                    if ($item->current_stock < $item->qty) {
                        return response()->json([
                            'status' => "0",
                            'message' => 'Insufficient stock',
                        ], Response::HTTP_BAD_REQUEST);
                    }

                    StockMovement::create([
                        'item_id' => $item->id,
                        'quantity' => -$item->qty,
                        'type' => StockMovementType::Deduction->value,
                    ]);

                    return response()->json([
                        'status' => "1",
                        'message' => 'Item Removed',
                    ], Response::HTTP_NOT_FOUND);
                } else {
                    return response()->json([
                        'status' => "0",
                        'message' => 'Item not found',
                    ], Response::HTTP_NOT_FOUND);
                }
            }

            return response()->json([
                'status' => "0",
                'message' => 'Invalid Action',
            ], Response::HTTP_UNPROCESSABLE_ENTITY);
        }
    }
}
