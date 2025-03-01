<?php

namespace App\Http\Controllers;

use App\Models\BusLocation;
use App\Models\Hardware;
use App\Models\Payment;
use App\Models\User;
use Illuminate\Http\Request;

class HardwareController extends Controller
{
    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        //
    }

    /**
     * Store a newly created resource in storage.
     */
    public function store(Request $request)
    {
        //
    }

    /**
     * Display the specified resource.
     */
    public function show(Request $request)
    {
        $request->validate([
            "card" => "sometimes|string"
        ]);

        if ($request->has('card')) {
            $user = User::where('card', $request->card)->first();
            if ($user) {
                $payment = Payment::where('user_id', $user->id)->get();
                if ($payment) {
                    $payedPayment = $payment->where('status', 'Payed');
                    if ($payedPayment->isNotEmpty()) {
                        $onePayment = $payedPayment->first();
                        $updatePayment = Payment::find($onePayment->id);
                        $updatePayment->status = 'Used';
                        $updatePayment->update();
                        return response()->json([
                            'card_allowed' => true,
                            'message' => 'Itike yabonetse',
                        ], 200);
                    }
                    $usedPayment = $payment->where('status', 'Used');
                    if ($usedPayment->isNotEmpty()) {
                        $onePayment = $usedPayment->first();
                        $updatePayment = Payment::find($onePayment->id);
                        $updatePayment->status = 'Used';
                        $updatePayment->update();
                        return response()->json([
                            'card_allowed' => true,
                            'message' => 'Yakoreshejwe',
                        ], 200);
                    } else {
                        return response()->json([
                            'card_allowed' => false,
                            'message' => 'Ntiyishyuye',
                        ], 200);
                    }
                } else {
                    return response()->json([
                        'card_allowed' => false,
                        'message' => 'Ntiyakatishije',
                    ], 200);
                }
            } else {
                return response()->json([
                    'card_allowed' => false,
                    'message' => 'Ikarita ntirimo',
                ], 200);
            }
        }
    }

    /**
     * Update the specified resource in storage.
     */
    public function update(Request $request, Hardware $hardware)
    {
        //
    }

    /**
     * Remove the specified resource from storage.
     */
    public function destroy(Hardware $hardware)
    {
        //
    }
}
